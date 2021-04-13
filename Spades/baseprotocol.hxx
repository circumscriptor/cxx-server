/**
 *
 * SpadesX
 *
 */

#pragma once

#include "connection.hxx"
#include "datastream.hxx"
#include "enums.hxx"
#include "map.hxx"

#include <array>
#include <chrono>
#include <enet/enet.h>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>

namespace spadesx {

class base_protocol
{
  public:
    /**
     * @brief Construct a new base protocol object
     *
     * @param max_players Max number of players
     */
    base_protocol(std::uint8_t max_players) :
        m_max_players{max_players},
        m_generator(std::random_device()()),
        m_distribution(0.F, 1.F)
    {
        m_map = std::make_unique<map>();
        if (!m_map) {
            throw std::runtime_error("failed to allocate map class");
        }

        m_connections.reserve(32);
        for (std::uint8_t i = 0; i < 32; ++i) {
            m_connections.emplace_back(i);
        }
    }

    virtual ~base_protocol() = default;

    /**
     * @brief Read map from file
     *
     * @param string File path
     */
    void load_map(std::string_view string)
    {
        m_map->read_from_file(string);
    }

    /**
     * @brief Default on_receive action
     *
     * @param peer Peer
     * @param packet Packet
     */
    void on_receive_default(ENetPeer* peer, ENetPacket* packet)
    {
        auto&       connection = peer_to_type<spadesx::connection>(peer);
        data_stream stream     = packet;

        switch (stream.read_type<packet_type>()) {
            case packet_type::existing_player:
            {
                std::cout << "[  LOG  ]: existing player from: " << static_cast<int>(connection.get_id()) << std::endl;
                if (connection.get_id() != stream.read_byte()) {
                    std::cout << "[WARNING]: existing player - invalid id received" << std::endl;
                }
                connection.m_team   = stream.read_type<team_type>();
                connection.m_weapon = stream.read_type<weapon_type>();
                connection.m_tool   = stream.read_type<tool_type>();
                connection.m_kills  = stream.read_int();
                stream.read_color3b(connection.m_color);

                auto length = stream.left();
                length      = (length > 15) ? 15 : length;
                connection.m_name.resize(length);
                stream.read_array(connection.m_name.data(), length);

                if (connection.m_alive) {
                    std::cout << "[WARNING]: player marked alive has sent existing player packet" << std::endl;
                    connection.m_alive = false;
                }

                connection.m_can_spawn = true;
            } break;
            default:
                std::cout << "[  LOG  ]: unhandled packet: " << std::hex << packet->data[0] << std::endl;
                break;
        }
    }

    /**
     * @brief Broadcast same data to multiple connections (except source)
     *
     * @param source Source connection
     * @param data Data
     * @param length Length of data
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast(const connection& source,
                   void*             data,
                   std::size_t       length,
                   bool              unsequenced = false,
                   std::uint8_t      channel     = 0)
    {
        for (auto& connection : m_connections) {
            if (connection == source) {
                continue;
            }
            if (connection) {
                connection.send_packet(data, length, unsequenced, channel);
            }
        }
    }

    /**
     * @brief Broadcast same data to multiple connections
     *
     * @param data Data
     * @param length Length of data
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast(void* data, std::size_t length, bool unsequenced = false, std::uint8_t channel = 0)
    {
        for (auto& connection : m_connections) {
            if (connection) {
                connection.send_packet(data, length, unsequenced, channel);
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
        victim.m_last_kill_killer = killer.get_id();
        victim.m_last_kill_type   = type;
        victim.m_respawn_time     = respawn_time;

        data_stream stream(m_cache_kill_action);
        victim.fill_kill_action(stream);
        broadcast(m_cache_kill_action.data(), m_cache_kill_action.size());
    }

    /**
     * @brief Broadcast create player
     *
     * @param connection Player to be created
     */
    void create(connection& connection)
    {
        data_stream stream(m_cache_create_player);
        auto        size = connection.fill_create_player(stream);
        broadcast(m_cache_create_player.data(), size);
    }

    /**
     * @brief Broadcast player left
     *
     * @param connection Connection
     */
    void leave(connection& connection)
    {
        data_stream stream(m_cache_player_left);
        stream.write_type(packet_type::player_left);
        stream.write_byte(connection.get_id());
        broadcast(connection, m_cache_player_left.data(), m_cache_player_left.size());
    }

    /**
     * @brief Broadcast world update
     *
     */
    void broadcast_world_update()
    {
        data_stream stream(m_cache_world_update);
        stream.write_type(packet_type::world_update);
        for (auto& connection : m_connections) {
            stream.write_vec3(connection.m_position);
            stream.write_vec3(connection.m_orientation);
        }
        broadcast(m_cache_world_update.data(), m_cache_world_update.size(), true);
    }

    /**
     * @brief Get players limit
     *
     * @return Limit
     */
    [[nodiscard]] std::uint8_t get_max_players() const
    {
        return m_max_players;
    }

    /**
     * @brief Get spawn location
     *
     */
    virtual void get_spawn_location(team_type team, entity_type /*entity*/, glm::vec3& v)
    {
    }

    /**
     * @brief On connect event
     *
     */
    virtual bool on_connect(ENetPeer* /*peer*/)
    {
        // check banned?
        return true;
    }

    /**
     * @brief On disconnect event
     *
     * @param peer Peer
     */
    virtual void on_disconnect(ENetPeer* peer)
    {
    }

    /**
     * @brief Handle packets
     *
     * @param peer Peer
     * @param packet Packet
     */
    virtual void on_receive(ENetPeer* peer, ENetPacket* packet)
    {
        on_receive_default(peer, packet);
    }

    /**
     * @brief On create event (spawn)
     *
     * @param connection Connection
     */
    virtual void on_create(connection& connection)
    {
        get_spawn_location(connection.m_team, entity_type::player, connection.m_position);
        create(connection);
    }

    /**
     * @brief On kill event
     *
     * @param killer Killer
     * @param victim Victim
     * @param type Kill type
     */
    virtual void on_kill(connection& killer, connection& victim, kill_type type)
    {
        // check if it is possible
        kill(killer, victim, type, m_respawn_time);
    }

    /**
     * @brief On start
     *
     */
    virtual void on_start()
    {
    }

    /**
     * @brief On update event
     *
     * @param connection Connection
     */
    virtual void on_update(connection& connection)
    {
        if (connection.m_can_spawn && !connection.m_alive && connection.m_respawn_time == 0) {
            connection.m_alive = true;
            on_create(connection);
        }
    }

    /**
     * @brief Send state
     *
     * @param connection Connection
     */
    virtual void on_send_state(connection& connection)
    {
    }

    /**
     * @brief Send existing players
     *
     * @param connection Connection
     */
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

    /**
     * @brief On map loading done
     *
     * @param connection Connection
     */
    void on_map_loading_done(connection& connection)
    {
        send_existing_players(connection);
        // send state
        on_send_state(connection);
        // loading done
        connection.set_state(state_type::connected);
    }

    /**
     * @brief Update connection
     *
     * @param connection Connection
     */
    void update_connection(connection& connection)
    {
        if (connection.is_connecting()) {
            if (!m_map_used) {

                if (m_map->has_changed()) {
                    m_map->write_and_compress(m_compressed_map);
                }

                // TODO: Allow multiple users to receive the same compressed map
                if (connection.send_map_start(m_compressed_map.size())) {
                    m_map_used      = true;
                    m_map_ownership = connection.get_id();
                    m_map_position  = m_compressed_map.begin();
                }

            } else if (m_map_used && m_map_ownership == connection.get_id()) {

                auto diff = m_compressed_map.end() - m_map_position;
                auto size = std::min(diff, static_cast<decltype(diff)>(8192));

                if (size == 0) {
                    on_map_loading_done(connection);
                    m_map_used = false;
                } else if (connection.send_map_chunk(m_map_position.base(), size)) {
                    m_map_position += size;
                }
            }
        } else if (connection.is_connected()) {
            on_update(connection);
        }
    }

    /**
     * @brief Start (on map start?)
     *
     */
    void start()
    {
        for (auto& connection : m_connections) {
            connection.reset_values();
        }

        on_start();
    }

    /**
     * @brief Update
     *
     */
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

        if (std::chrono::duration<double>(now - m_world_update_timer).count() >= m_world_update_delta) {
            m_world_update_timer = now;
            // update world
            // ...
            broadcast_world_update();
        }
    }

    /**
     * @brief Try connecting peer
     *
     * @param peer Peer to be connected
     */
    void try_connect(ENetPeer* peer)
    {
        if (peer->eventData != static_cast<std::uint32_t>(version_type::v75)) {
            enet_peer_disconnect(peer, static_cast<std::uint32_t>(reason_type::wrong_protocol_version));
            return;
        }

        if (!available()) {
            enet_peer_disconnect(peer, static_cast<std::uint32_t>(reason_type::server_full));
            return;
        }

        connection& connection = next_connection();
        if (!on_connect(peer)) {
            // reset?
            return;
        }
        m_num_players++;
        peer->data = &connection;
    }

    /**
     * @brief Try disconnecting peer
     *
     * @param peer Peer to be disconnected
     */
    void try_disconnect(ENetPeer* peer)
    {
        auto& connection = peer_to_type<spadesx::connection>(peer);

        on_disconnect(peer);

        leave(connection);
        peer->data = nullptr;
        connection.reset_values();
        connection.set_state(state_type::disconnected);
        m_num_players--;
    }

    /**
     * @brief Set fog color
     *
     * @param color Color (RGB)
     */
    void set_fog_color(const color3b& color)
    {
        m_fog_color = color;
    }

    /**
     * @brief Set world update delta time
     *
     * @param delta New delta time
     */
    void set_world_update_delta(double delta)
    {
        if (m_world_update_delta >= 0) {
            m_world_update_delta = delta;
        }
    }

  protected:
    /**
     * @brief Check whether server is not full
     *
     * @return true If server is not full
     * @return false If serever is full
     */
    [[nodiscard]] bool available() const
    {
        return m_num_players < m_max_players;
    }

    /**
     * @brief Get next free connection
     *
     * @return Connection
     */
    connection& next_connection()
    {
        for (auto& connection : m_connections) {
            if (!connection) {
                return connection;
            }
        }
        throw std::runtime_error("failed to get next free connection");
    }

    /**
     * @brief Convert peer to type
     *
     * @tparam T Type
     * @param peer Peer
     * @return Reference
     */
    template<typename T>
    T& peer_to_type(ENetPeer* peer) const
    {
        return *reinterpret_cast<T*>(peer->data);
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

    std::uint8_t m_max_players;     //!< Maximal number of players
    std::uint8_t m_num_players{0};  //!< Current number of players
    std::uint8_t m_respawn_time{0}; //!< Current respawn time

    std::unique_ptr<map> m_map;       //!< Map
    color3b              m_fog_color; //!< Fog color

    double m_world_update_delta{0.1};

    std::array<std::uint8_t, 5>   m_cache_kill_action;
    std::array<std::uint8_t, 32>  m_cache_create_player;
    std::array<std::uint8_t, 769> m_cache_world_update;
    std::array<std::uint8_t, 104> m_cache_state_data;
    std::array<std::uint8_t, 2>   m_cache_player_left;
    std::vector<char>             m_compressed_map;
    std::vector<char>::iterator   m_map_position;
    bool                          m_map_used{false};
    std::uint8_t                  m_map_ownership;

    std::vector<connection> m_connections; //!< Player connections

    std::mt19937                                       m_generator;          //!< Random number generator
    std::uniform_real_distribution<float>              m_distribution;       //!< Real number distribution
    std::chrono::time_point<std::chrono::steady_clock> m_respawn_timer;      //!< Respawn timer
    std::chrono::time_point<std::chrono::steady_clock> m_world_update_timer; //!< World update timer
};

} // namespace spadesx
