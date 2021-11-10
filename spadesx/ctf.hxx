/**
 * @file ctf.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "data/team.hxx"
#include "protocol.hxx"

namespace spadesx {

/**
 * @brief CTF team data
 *
 */
class ctf_team_data : public team_data
{
  public:
    /**
     * @brief Construct a new ctf_team_data object
     *
     * @param team Team
     */
    ctf_team_data(team_type team) : team_data{team}, m_intel{team}, m_base{team}
    {
    }

    /**
     * @brief Get the intel object
     *
     * @return Intel
     */
    constexpr intel_data& intel() noexcept
    {
        return m_intel;
    }

    /**
     * @brief Get the intel object
     *
     * @return Intel
     */
    [[nodiscard]] constexpr const intel_data& intel() const noexcept
    {
        return m_intel;
    }

    /**
     * @brief Get the base object
     *
     * @return Base
     */
    constexpr base_data& base() noexcept
    {
        return m_base;
    }

    /**
     * @brief Get the base object
     *
     * @return Base
     */
    [[nodiscard]] constexpr const base_data& base() const noexcept
    {
        return m_base;
    }

  private:
    intel_data m_intel; //!< Intel
    base_data  m_base;  //!< Base
};

/**
 * @brief CTF protocol
 *
 */
class ctf_protocol : public protocol
{
  public:
    /**
     * @brief Construct a new ctf_protocol object
     *
     * @param max_players Max number of players
     */
    ctf_protocol(std::uint8_t max_players) : protocol(max_players), m_teams{team_type::a, team_type::b}
    {
    }

    // TODO: Add more general spawn location function
    void get_spawn_location(team_type team, entity& entity) override
    {
        get_spawn(team).get(get_random_float(), get_random_float(), entity.m_position);
        entity.m_position.z = m_map->get_height_f(entity.m_position.x, entity.m_position.y);
    }

    void on_start() override
    {
        for (auto& team : m_teams) {
            team.intel().reset();
            team.score().reset();
        }
        get_spawn_location(team_type::a, m_teams[0].base());
        get_spawn_location(team_type::b, m_teams[1].base());
        get_spawn_location(team_type::a, m_teams[0].intel());
        get_spawn_location(team_type::b, m_teams[1].intel());
    }

    void on_kill(connection& killer, connection& victim, kill_type type, std::uint8_t respawn_time) override
    {
        server_handler::on_kill(killer, victim, type, respawn_time);
        if (victim.team() != team_type::spectator) {
            auto& enemy_team = get_enemy_team(victim.team());
            if (check_and_drop_intel(victim, enemy_team, victim.m_position)) {
                std::cout << "[  LOG  ]: player " << victim.name() << " dropped intel (reason: killed by "
                          << killer.name() << ")" << std::endl;
            }
        }
    }

    void on_disconnect(connection& connection) override
    {
        if (connection.team() != team_type::spectator) {
            auto& enemy_team = get_enemy_team(connection.team());
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
        team.intel().pick(enemy);
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
        if (!enemy_team.intel().is_taken()) {
            if (enemy_team.intel().distance(source) <= m_intel_pickup_distance) {
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
            enemy_team.intel().drop();
            get_spawn_location(enemy_team.team(), enemy_team.intel());
        } else {
            enemy_team.intel().drop(position);
        }
        broadcast_intel_drop(source, enemy_team.intel().m_position);
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
        if (enemy_team.intel().is_held_by(source)) {
            drop_intel(source, enemy_team, position);
            return true;
        }
        return false;
    }

    /**
     * @brief Broadcast intel capture
     *
     * @param source Source connection
     * @param team Team data
     * @param enemy_team Enemy team data
     * @param position Reset position
     */
    void capture_intel(connection& source, ctf_team_data& team, ctf_team_data& enemy_team, const glm::vec3& position)
    {
        source.score().increase();
        enemy_team.intel().drop(position);
        broadcast_intel_capture(source, team.score().increase().has_reached(m_score_limit));
        broadcast_move_object(enemy_team.intel().id(), enemy_team.team(), position);
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
        if (enemy_team.intel().is_held_by(source)) {
            if (team.base().distance(source) <= m_base_trigger_distance) {
                get_spawn_location(enemy_team.team(), enemy_team.intel());
                capture_intel(source, team, enemy_team, enemy_team.intel().m_position);
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
            get_spawn_location(connection.team(), connection);
            broadcast_create(connection);
        }
        if (connection.m_alive) {
            if (connection.team() != team_type::spectator) {
                auto& team = get_team(connection.team());

                if (check_restock(connection, team.base())) {
                    restock(connection, m_restock_time);
                }

                auto& enemy_team = get_enemy_team(connection.team());

                if (check_and_pickup_intel(connection, enemy_team)) {
                    std::cout << "[  LOG  ]: intel taken by " << connection.name() << std::endl;
                }

                if (check_and_capture_intel(connection, team, enemy_team)) {
                    std::cout << "[  LOG  ]: intel captured by " << connection.name() << std::endl;
                    if (team.score().has_reached(m_score_limit)) {
                        std::cout << "[  LOG  ]: end game" << std::endl;
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
        stream.write_byte(connection.id());
        stream.write_color3b(m_fog_color);
        stream.write_color3b(m_teams[0].color());
        stream.write_color3b(m_teams[1].color());
        stream.write_array(m_teams[0].name().data(), 10);
        stream.write_array(m_teams[1].name().data(), 10);
        stream.write_type(mode_type::ctf);
        // CTF
        stream.write_byte(m_teams[0].score().to_byte());
        stream.write_byte(m_teams[1].score().to_byte());
        stream.write_byte(m_score_limit);
        stream.write_byte(intel_flags());
        if (!m_teams[0].intel().is_taken()) {
            stream.write_vec3(m_teams[0].intel().m_position);
        } else {
            stream.write_byte(m_teams[0].intel().holder());
            stream.skip(11);
        }
        if (!m_teams[1].intel().is_taken()) {
            stream.write_vec3(m_teams[1].intel().m_position);
        } else {
            stream.write_byte(m_teams[1].intel().holder());
            stream.skip(11);
        }
        stream.write_vec3(m_teams[0].base().m_position);
        stream.write_vec3(m_teams[1].base().m_position);
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
    float m_intel_pickup_distance{3.F}; //!< Intel pickup distance

    /**
     * @brief Generate intel flags
     *
     * @return Intel flags
     */
    [[nodiscard]] std::uint8_t intel_flags() const noexcept
    {
        std::uint8_t flags = 0;
        flags |= (m_teams[0].intel().is_taken()) ? 1 : 0;
        flags |= (m_teams[1].intel().is_taken()) ? 2 : 0;
        return flags;
    }

  private:
    std::uint8_t                 m_score_limit{10};
    std::array<spawn, 3>         m_spawns;
    std::array<ctf_team_data, 2> m_teams;
};

} // namespace spadesx
