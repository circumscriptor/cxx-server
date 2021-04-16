/**
 *
 * SpadesX
 *
 */

#pragma once

#include "baseprotocol.hxx"
#include "data/base.hxx"
#include "data/enums.hxx"
#include "data/team.hxx"
#include "spawn.hxx"

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

    intel_data m_intel; //!< Intel
    base_data  m_base;  //!< Base
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
            if (enemy_team.m_intel.is_held_by(victim.get_id())) {
                enemy_team.m_intel.drop(victim.m_position);

                auto x = static_cast<std::uint32_t>(enemy_team.m_intel.m_position.x);
                auto y = static_cast<std::uint32_t>(enemy_team.m_intel.m_position.y);
                auto z = static_cast<std::uint32_t>(enemy_team.m_intel.m_position.z);
                broadcast_intel_drop(victim, x, y, z);
            }
        }
    }

    void on_update(connection& connection) override
    {
        if (connection.m_can_spawn && !connection.m_alive && connection.m_respawn_time == 0) {
            connection.m_alive = true;
            get_spawn_location(connection.m_team, entity_type::player, connection.m_position);
            broadcast_create(connection);
        }
        if (connection.m_alive && connection.m_respawn_time == 0) { // reuse respawn timer for restock
            if (connection.m_team != team_type::spectator) {
                auto& team       = get_team(connection.m_team);
                auto& enemy_team = get_enemy_team(connection.m_team);
                if (team.m_base.distance(connection) <= 3.0) {
                    if (enemy_team.m_intel.is_held_by(connection.get_id())) {
                        team.add_score();
                        broadcast_intel_capture(connection, team.m_score == m_score_limit);

                        enemy_team.m_intel.drop();
                        get_spawn_location(enemy_team.type(), entity_type::intel, enemy_team.m_intel.m_position);

                        auto x = static_cast<std::uint32_t>(enemy_team.m_intel.m_position.x);
                        auto y = static_cast<std::uint32_t>(enemy_team.m_intel.m_position.y);
                        auto z = static_cast<std::uint32_t>(enemy_team.m_intel.m_position.z);
                        broadcast_intel_drop(connection, x, y, z);
                    } else {
                        connection.m_respawn_time = 15;
                        broadcast_restock(connection);
                    }
                }
                if (!enemy_team.m_intel.is_taken() && enemy_team.m_intel.distance(connection) <= 2.0) {
                    enemy_team.m_intel.pick(connection.get_id());
                    broadcast_intel_pickup(connection);
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
        stream.write_byte(m_teams[0].m_score + 9);
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
