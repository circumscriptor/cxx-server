/**
 *
 * SpadesX
 *
 */

#pragma once

#include "base.hxx"
#include "baseprotocol.hxx"
#include "data/enums.hxx"
#include "data/team.hxx"
#include "data/weapon.hxx"
#include "intel.hxx"
#include "spawn.hxx"

#include <glm/geometric.hpp>
#include <glm/gtx/string_cast.hpp>

namespace spadesx {

/**
 * @brief CTF team data
 *
 */
class ctf_team_data : public team_data
{
  public:
    ctf_team_data(team_type type) : team_data{type}
    {
    }

    intel m_intel; //!< Intel
    base  m_base;  //!< Base
};

/**
 * @brief CTF protocol
 *
 */
class ctf_protocol : public base_protocol
{
  public:
    ctf_protocol(std::uint8_t max_players) : base_protocol(max_players), m_teams{team_type::a, team_type::b}
    {
    }

    void get_spawn_location(team_type team, entity_type /*entity*/, glm::vec3& v) override
    {
        get_spawn(team).get(get_random_float(), get_random_float(), v);
        v.z = m_map->get_height(v.x, v.y);
    }

    void on_start() override
    {
        for (auto& team : m_teams) {
            team.m_intel.reset();
            team.reset_score();
        }
        get_spawn_location(team_type::a, entity_type::base, m_teams[0].m_base.m_position);
        get_spawn_location(team_type::b, entity_type::base, m_teams[1].m_base.m_position);
        get_spawn_location(team_type::a, entity_type::intel, m_teams[0].m_intel.m_position);
        get_spawn_location(team_type::b, entity_type::intel, m_teams[1].m_intel.m_position);
    }

    void on_kill(connection& killer, connection& victim, kill_type type, std::uint8_t respawn_time) override
    {
        server_handler::on_kill(killer, victim, type, respawn_time);
        if (victim.m_team != team_type::spectator) {
            auto& enemy_team = get_enemy_team(victim.m_team);
            if (check_and_drop_intel(victim, enemy_team, victim.m_position)) {
                std::cout << "[  LOG  ]: player " << victim.name() << " dropped intel (reason: killed by "
                          << killer.name() << ")" << std::endl;
            }
        }
    }

    void on_disconnect(connection& connection) override
    {
        if (connection.m_team != team_type::spectator) {
            auto& enemy_team = get_enemy_team(connection.m_team);
            if (check_and_drop_intel(connection, enemy_team, connection.m_position)) {
                std::cout << "[  LOG  ]: player " << connection.name() << " dropped intel (reason: disconnected)"
                          << std::endl;
            }
        }
    }

    /**
     * @brief Broadcast intel pickup
     *
     * @param team Team
     * @param enemy Enemy player
     */
    void pickup_intel(ctf_team_data& team, const connection& enemy)
    {
        team.m_intel.pick(enemy);
        broadcast_intel_pickup(enemy);
    }

    /**
     * @brief Check and broadcast intel pickup
     *
     * @param source source Source connection
     * @param enemy_team Enemy team data
     * @return true On success
     */
    bool check_and_pickup_intel(const connection& source, ctf_team_data& enemy_team)
    {
        if (!enemy_team.m_intel.is_taken()) {
            if (enemy_team.m_intel.distance(source) <= m_intel_pickup_distance) {
                pickup_intel(enemy_team, source);
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Drop intel at position
     *
     * @param source Source connection
     * @param enemy_team Enemy team
     * @param position New intel postion
     */
    void drop_intel(const connection& source, ctf_team_data& enemy_team, const glm::vec3& position)
    {
        // check boundaries
        if (std::uint32_t(position.x) >= 512 || std::uint32_t(position.y) >= 512 || std::uint32_t(position.z) >= 64) {
            enemy_team.m_intel.drop();
            get_spawn_location(enemy_team.type(), entity_type::intel, enemy_team.m_intel.m_position);
        } else {
            enemy_team.m_intel.drop(position);
        }
        broadcast_intel_drop(source, enemy_team.m_intel.m_position);
    }

    /**
     * @brief Check and drop intel
     *
     * @param source Source connection
     * @param enemy_team Enemy team data
     * @param position New intel position
     * @return true On success
     */
    bool check_and_drop_intel(const connection& source, ctf_team_data& enemy_team, const glm::uvec3& position)
    {
        if (enemy_team.m_intel.is_held_by(source)) {
            drop_intel(source, enemy_team, position);
            return true;
        }
        return false;
    }

    /**
     * @brief Broadcast intel capture
     *
     * @param source Source connection
     * @param enemy_team Enemy team data
     * @param winning Is this a wining capture
     * @param position Reset position
     */
    void capture_intel(connection& source, ctf_team_data& team, ctf_team_data& enemy_team, const glm::vec3& position)
    {
        enemy_team.m_intel.drop(position);
        broadcast_intel_capture(source, team.add_score().has_reached_score(m_score_limit));
        // broadcast_intel_drop(source, position);
        if (enemy_team.type() == team_type::a) {
            broadcast_move_object(object_id::team_a_intel, enemy_team.type(), position);
        } else {
            broadcast_move_object(object_id::team_b_intel, enemy_team.type(), position);
        }
    }

    /**
     * @brief Check and broadcast intel capture
     *
     * @param source Source connection
     * @param team Team data
     * @param enemy_team Enemy team data
     * @return true On success
     */
    bool check_and_capture_intel(connection& source, ctf_team_data& team, ctf_team_data& enemy_team)
    {
        if (enemy_team.m_intel.is_held_by(source)) {
            if (team.m_base.distance(source) <= m_base_trigger_distance) {
                get_spawn_location(enemy_team.type(), entity_type::intel, enemy_team.m_intel.m_position);
                capture_intel(source, team, enemy_team, enemy_team.m_intel.m_position);
                return true;
            }
        }
        return false;
    }

    /**
     * @brief On update
     *
     * @param connection Connection to be updated
     */
    void on_update(connection& connection) override
    {
        if (connection.m_can_spawn && !connection.m_alive && connection.m_respawn_time == 0) {
            connection.m_alive = true;
            get_spawn_location(connection.m_team, entity_type::player, connection.m_position);
            broadcast_create(connection);
        }
        if (connection.m_alive) {
            if (connection.m_team != team_type::spectator) {
                auto& team = get_team(connection.m_team);

                if (check_restock(connection, team.m_base)) {
                    restock(connection, m_restock_time);
                }

                auto& enemy_team = get_enemy_team(connection.m_team);

                if (check_and_pickup_intel(connection, enemy_team)) {
                    std::cout << "[  LOG  ]: intel taken by " << connection.name() << std::endl;
                }

                if (check_and_capture_intel(connection, team, enemy_team)) {
                    std::cout << "[  LOG  ]: intel captured by " << connection.name() << std::endl;
                    if (team.has_reached_score(m_score_limit)) {
                        std::cout << "[  LOG  ]: end game" << std::endl;
                        team.m_score       = 0;
                        enemy_team.m_score = 0;
                    }
                }
            }
        }
    }

    /**
     * @brief Send state data
     *
     * @param connection Connection
     */
    bool on_send_state(connection& connection) override
    {
        data_stream stream{m_cache, packet::state_data_size};
        stream.write_type(packet_type::state_data);
        stream.write_byte(connection.get_id());
        stream.write_color3b(m_fog_color);
        stream.write_color3b(m_teams[0].m_color);
        stream.write_color3b(m_teams[1].m_color);
        stream.write_array(m_teams[0].name().data(), 10);
        stream.write_array(m_teams[1].name().data(), 10);
        stream.write_type(mode_type::ctf);
        // CTF
        stream.write_byte(m_teams[0].m_score);
        stream.write_byte(m_teams[1].m_score);
        stream.write_byte(m_score_limit);
        stream.write_byte(intel_flags());
        if (!m_teams[0].m_intel.is_taken()) {
            stream.write_vec3(m_teams[0].m_intel.m_position);
        } else {
            stream.write_byte(m_teams[0].m_intel.holder());
            stream.skip(11);
        }
        if (!m_teams[1].m_intel.is_taken()) {
            stream.write_vec3(m_teams[1].m_intel.m_position);
        } else {
            stream.write_byte(m_teams[1].m_intel.holder());
            stream.skip(11);
        }
        stream.write_vec3(m_teams[0].m_base.m_position);
        stream.write_vec3(m_teams[1].m_base.m_position);
        return connection.send_packet(m_cache, packet::state_data_size);
    }

    /**
     * @brief Get the spawn object
     *
     * @param team Team
     * @return Spawn
     */
    spawn& get_spawn(team_type team)
    {
        switch (team) {
            case team_type::a:
                return m_spawns[0];
            case team_type::b:
                return m_spawns[1];
            default:
                return m_spawns[2];
        }
    }

    /**
     * @brief Get team data
     *
     * @param team Team
     * @return Team data
     */
    ctf_team_data& get_team(team_type team)
    {
        switch (team) {
            case team_type::a:
                return m_teams[0];
            case team_type::b:
                return m_teams[1];
            default:
                throw std::runtime_error("no team data for the given team");
        }
    }

    /**
     * @brief Get enemy team data
     *
     * @param team Team
     * @return Enemy team data
     */
    ctf_team_data& get_enemy_team(team_type team)
    {
        switch (team) {
            case team_type::a:
                return m_teams[1];
            case team_type::b:
                return m_teams[0];
            default:
                throw std::runtime_error("no team data for the given team");
        }
    }

  protected:
    float m_intel_pickup_distance{3.F};

    /**
     * @brief Generate intel flags
     *
     * @return Intel flags
     */
    [[nodiscard]] std::uint8_t intel_flags() const noexcept
    {
        std::uint8_t flags = 0;
        flags |= (m_teams[0].m_intel.is_taken()) ? 1 : 0;
        flags |= (m_teams[1].m_intel.is_taken()) ? 2 : 0;
        return flags;
    }

  private:
    std::uint8_t                 m_score_limit{10};
    std::array<spawn, 3>         m_spawns;
    std::array<ctf_team_data, 2> m_teams;
};

} // namespace spadesx
