/**
 *
 * SpadesX
 *
 */

#pragma once

#include "connection.hxx"
#include "map.hxx"
#include "spawn.hxx"

#include <array>
#include <chrono>
#include <enet/enet.h>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>

namespace spadesx {

struct team_data
{
    color3b      m_color;
    std::string  m_name;
    std::uint8_t m_score;
    bool         m_intel_taken;
    std::uint8_t m_intel_holder; // enemy team
    glm::vec3    m_intel;
    glm::vec3    m_base;

    void set_name(const std::string& name)
    {
        if (name.length() > 10) {
            m_name.assign(name.begin(), name.begin() + 16);
        } else {
            m_name = name;
        }
    }
};

class protocol
{
  public:
    protocol(std::uint8_t max_players) :
        m_max_players{max_players},
        m_generator(std::random_device()()),
        m_distribution(0.F, 1.F)
    {
        m_map = std::make_unique<map>();
        if (!m_map) {
            throw std::runtime_error("failed to allocate map class");
        }

        std::uint8_t i = 0;
        for (auto& connection : m_connections) {
            connection.m_id = i++;
        }
    }

    virtual void on_start()
    {
    }

    virtual void on_connect(ENetPeer* peer)
    {
    }

    virtual void on_disconnect(ENetPeer* peer)
    {
    }

    virtual void on_receive(ENetPeer* peer, ENetPacket* packet)
    {
    }

    /**
     * @brief Get spawn location
     *
     * @param team Team
     * @param v Output vector
     */
    virtual void get_spawn_location(team_type team, entity_type /*entity*/, glm::vec3& v)
    {
        get_spawn(team).get(get_random_float(), get_random_float(), v);
        v.z = m_map->get_height(v.x, v.y);
    }

    virtual void on_create(connection& connection)
    {
        get_spawn_location(connection.m_team, entity_type::player, connection.m_position);
        create(connection);
    }

    virtual void on_kill(connection& killer, connection& victim, kill_type type)
    {
        kill(killer, victim, type, m_respawn_time);
    }

    /**
     * @brief Broadcast same data to multiple connections
     *
     * @param source Source connection
     * @param data Data
     * @param length Length of data
     * @param include_source If true sends back to source
     * @param unsenquenced If true sets unsenquenced flag
     * @param channel Channel
     */
    void broadcast(const connection& source,
                   void*             data,
                   std::size_t       length,
                   bool              include_source = true,
                   bool              unsenquenced   = false,
                   std::uint8_t      channel        = 0)
    {
        for (auto& connection : m_connections) {
            if (!include_source && connection == source) {
                continue;
            }
            if (connection) {
                connection.send_packet(data, length, unsenquenced, channel);
            }
        }
    }

    /**
     * @brief Broadcast same data to multiple connections
     *
     * @param data Data
     * @param length Length of data
     * @param unsenquenced If true sets unsenquenced flag
     * @param channel Channel
     */
    void broadcast(void* data, std::size_t length, bool unsenquenced = false, std::uint8_t channel = 0)
    {
        for (auto& connection : m_connections) {
            if (connection) {
                connection.send_packet(data, length, unsenquenced, channel);
            }
        }
    }

    /**
     * @brief Broadcast kill action
     *
     * @param killer Killer
     * @param victim Victim
     * @param type Kill type
     * @param respawn_time Respawn time
     */
    void kill(connection& killer, connection& victim, kill_type type, std::uint8_t respawn_time)
    {
        victim.m_alive            = false;
        victim.m_last_kill_killer = killer.m_id;
        victim.m_last_kill_type   = type;
        victim.m_respawn_time     = respawn_time;

        data_stream stream(m_cache.m_kill_action.begin(), m_cache.m_kill_action.end());
        stream.write_type(packet_type::kill_action);
        stream.write_byte(victim.m_id);
        stream.write_byte(victim.m_last_kill_killer);
        stream.write_type(victim.m_last_kill_type);
        stream.write_byte(victim.m_respawn_time);
        broadcast(killer, m_cache.m_kill_action.data(), m_cache.m_kill_action.size());
    }

    /**
     * @brief Broadcast create player
     *
     * @param connection Player to be created
     */
    void create(connection& connection)
    {
        data_stream stream(m_cache.m_create_player.begin(), m_cache.m_create_player.end());
        stream.write_type(packet_type::create_player);
        stream.write_byte(connection.m_id);
        stream.write_type(connection.m_weapon);
        stream.write_type(connection.m_team);
        stream.write_vec3(connection.m_position);
        stream.write_array(connection.m_name.c_str(), connection.m_name.length());
        broadcast(connection,
                  m_cache.m_create_player.data(),
                  m_cache.m_create_player.size() - 16 + connection.m_name.length(),
                  true);
    }

    void broadcast_world_update()
    {
        data_stream stream(m_cache.m_world_update.begin(), m_cache.m_world_update.end());
        stream.write_type(packet_type::world_update);
        for (auto& connection : m_connections) {
            stream.write_vec3(connection.m_position);
            stream.write_vec3(connection.m_orientation);
        }
        broadcast(m_cache.m_world_update.data(), m_cache.m_world_update.size(), true);
    }

    void send_existing_players(connection& connection)
    {
        for (auto& other : m_connections) {
            if (connection != other && other.is_connected()) { // send only connected players
                connection.send_existing_player(other);
            }
        }
        for (auto& other : m_connections) {
            if (connection != other && other.is_connected() && !other.m_alive) {
                connection.send_kill_action(other);
            }
        }
    }

    bool send_state(connection& connection)
    {
        data_stream packet(m_cache.m_state_data.begin(), m_cache.m_state_data.end());
        packet.write_type(packet_type::state_data);
        packet.write_byte(connection.m_id);
        packet.write_color3b(m_fog_color);
        packet.write_color3b(m_teams[0].m_color);
        packet.write_color3b(m_teams[1].m_color);
        packet.write_array(m_teams[0].m_name.c_str(), 10);
        packet.write_array(m_teams[1].m_name.c_str(), 10);
        packet.write_type(mode_type::ctf);
        // CTF
        packet.write_byte(m_teams[0].m_score + 9);
        packet.write_byte(m_teams[1].m_score);
        packet.write_byte(m_score_limit);
        packet.write_byte(intel_flags());
        if (!m_teams[0].m_intel_taken) {
            packet.write_vec3(m_teams[0].m_intel);
        } else {
            packet.write_byte(m_teams[0].m_intel_holder);
            packet.skip(11);
        }
        if (!m_teams[1].m_intel_taken) {
            packet.write_vec3(m_teams[1].m_intel);
        } else {
            packet.write_byte(m_teams[1].m_intel_holder);
            packet.skip(11);
        }
        packet.write_vec3(m_teams[1].m_base);
        packet.write_vec3(m_teams[1].m_base);
        return connection.send_packet(m_cache.m_state_data.data(), m_cache.m_state_data.size());
    }

    void update_connection(connection& connection)
    {
        if (connection.is_connecting()) {
            if (!m_cache.m_map_used) {
                if (m_cache.m_map_changed) {
                    m_map->write_and_compress(m_cache.m_compressed_map);
                }
                if (connection.send_map_start(m_cache.m_compressed_map.size())) {
                    m_cache.m_map_used      = true;
                    m_cache.m_map_ownership = connection.m_id;
                    m_cache.m_map_position  = m_cache.m_compressed_map.begin();
                }
            } else if (m_cache.m_map_used && m_cache.m_map_ownership == connection.m_id) {
                auto diff = m_cache.m_compressed_map.end() - m_cache.m_map_position;
                auto size = std::min(diff, static_cast<decltype(diff)>(8192));
                if (size == 0) {
                    send_existing_players(connection);
                    send_state(connection);
                    // loading done
                    m_cache.m_map_used = false;
                    connection.m_state = state_type::connected;
                } else if (connection.send_map_chunk(m_cache.m_map_position.base(), size)) {
                    m_cache.m_map_position += size;
                }
            }
        } else if (connection.is_connected()) {
            if (connection.m_can_spawn && !connection.m_alive && connection.m_respawn_time == 0) {
                connection.m_alive = true;
                on_create(connection);
            }
        }
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

    team_data& get_team(team_type team)
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

    map& get_map()
    {
        return *m_map;
    }

    void update()
    {
        for (auto& connection : m_connections) {
            if (connection) {
                update_connection(connection);
            }
        }

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration<double>(now - m_respawn_timer).count() >= 1) {
            m_respawn_timer = now;
            for (auto& connection : m_connections) {
                if (connection.m_respawn_time != 0) {
                    connection.m_respawn_time--;
                }
            }
        }

        if (std::chrono::duration<double>(now - m_world_update_timer).count() >= 0.1) {
            m_world_update_timer = now;
            // update world
            // ...
            broadcast_world_update();
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
        flags |= (m_teams[0].m_intel_taken) ? 1 : 0;
        flags |= (m_teams[1].m_intel_taken) ? 2 : 0;
        return flags;
    }

    /**
     * @brief Get random float number in range 0 to 1
     *
     * @return Float number
     */
    float get_random_float()
    {
        return m_distribution(m_generator);
    }

  private:
    std::uint8_t               m_max_players;    //!< Maximal number of players
    std::uint8_t               m_num_players{0}; //!< Current number of players
    std::uint8_t               m_score_limit{10};
    std::uint8_t               m_respawn_time{0};
    std::array<spawn, 3>       m_spawns;
    std::array<connection, 32> m_connections;
    std::array<team_data, 2>   m_teams;
    std::unique_ptr<map>       m_map;
    color3b                    m_fog_color;

    struct cache
    {
        std::array<std::uint8_t, 5>         m_kill_action;
        std::array<std::uint8_t, 32>        m_create_player;
        std::array<std::uint8_t, 769>       m_world_update;
        std::array<std::uint8_t, 104>       m_state_data;
        std::vector<std::uint8_t>           m_compressed_map;
        std::vector<std::uint8_t>::iterator m_map_position;
        bool                                m_map_changed{true};
        bool                                m_map_used{false};
        std::uint8_t                        m_map_ownership;
    } m_cache;

    std::mt19937                                       m_generator;
    std::uniform_real_distribution<float>              m_distribution;
    std::chrono::time_point<std::chrono::steady_clock> m_respawn_timer;
    std::chrono::time_point<std::chrono::steady_clock> m_world_update_timer;
};

} // namespace spadesx
