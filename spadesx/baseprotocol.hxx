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
#include "weapondata.hxx"

#include <array>
#include <chrono>
#include <enet/enet.h>
#include <glm/glm.hpp>
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
     * @param connection Connection
     * @param type Packet type
     * @param strema Packet stream
     */
    void on_receive_default(connection& connection, packet_type type, data_stream& stream)
    {
        switch (type) {
            case packet_type::position_data:
            {
                stream.read_vec3(connection.m_position);
            } break;
            case packet_type::orientation_data:
            {
                stream.read_vec3(connection.m_orientation);
            } break;
            case packet_type::input_data:
            {
                if (connection.get_id() != stream.read_byte()) {
                    std::cout << "[WARNING]: input data - invalid id received" << std::endl;
                }
                connection.set_input(stream.read_byte());

                // broadcast
                data_stream output(m_cache_input_data);
                connection.fill_input_data(output);
                broadcast(connection, m_cache_input_data);
            } break;
            case packet_type::weapon_input:
            {
                if (connection.get_id() != stream.read_byte()) {
                    std::cout << "[WARNING]: weapon input - invalid id received" << std::endl;
                }
                connection.set_weapon_input(stream.read_byte());

                // broadcast
                data_stream output(m_cache_weapon_input);
                connection.fill_weapon_input(output);
                broadcast(connection, m_cache_weapon_input);
            } break;
            case packet_type::hit_packet:
            {
                auto target = stream.read_byte();
                auto type   = stream.read_type<hit_type>();
                if (target < m_num_players) {
                    auto& victim = m_connections[target];
                    if (victim.is_connected()) {
                        on_player_hit(connection, victim, type);
                    } else {
                        std::cout << "[WARNING]: player hit is not connected" << std::endl;
                    }
                } else {
                    std::cout << "[WARNING]: hit packet - invalid hit id received" << std::endl;
                }
            } break;
            case packet_type::grenade_packet:
            {
                if (connection.get_id() != stream.read_byte()) {
                    std::cout << "[WARNING]: grenade packet - invalid id received" << std::endl;
                }

                float     fuse;
                glm::vec3 position;
                glm::vec3 velocity;

                fuse = stream.read_float();
                stream.read_vec3(position);
                stream.read_vec3(velocity);

                on_grenade_throw(connection, position, velocity, fuse);
            } break;
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
                std::cout << "[  LOG  ]: unhandled packet: " << static_cast<std::uint32_t>(type) << std::endl;
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
                   const void*       data,
                   std::size_t       length,
                   bool              unsequenced = false,
                   std::uint8_t      channel     = 0)
    {
        for (auto& connection : m_connections) {
            if (connection == source) {
                continue;
            }
            if (!connection.is_disconnected()) {
                connection.send_packet(data, length, unsequenced, channel);
            }
        }
    }

    /**
     * @brief Broadcast same data to multiple connections (except source) - cached array
     *
     * @param source Source connection
     * @param data Data
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    template<std::size_t N>
    void broadcast(const connection&                  source,
                   const std::array<std::uint8_t, N>& data,
                   bool                               unsequenced = false,
                   std::uint8_t                       channel     = 0)
    {
        broadcast(source, data.data(), data.size(), unsequenced, channel);
    }

    /**
     * @brief Broadcast same data to multiple connections
     *
     * @param data Data
     * @param length Length of data
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast(const void* data, std::size_t length, bool unsequenced = false, std::uint8_t channel = 0)
    {
        for (auto& connection : m_connections) {
            if (!connection.is_disconnected()) {
                connection.send_packet(data, length, unsequenced, channel);
            }
        }
    }

    /**
     * @brief Broadcast same data to multiple connections - cached array
     *
     * @param data Data
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    template<std::size_t N>
    void broadcast(const std::array<std::uint8_t, N>& data, bool unsequenced = false, std::uint8_t channel = 0)
    {
        broadcast(data.data(), data.size(), unsequenced, channel);
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
        victim.m_last_kill_killer = killer.get_id();
        victim.m_last_kill_type   = type;
        victim.m_respawn_time     = respawn_time;
        victim.reset_death();

        data_stream stream(m_cache_kill_action);
        victim.fill_kill_action(stream);
        broadcast(m_cache_kill_action);
    }

    /**
     * @brief Throw grenade
     *
     * @param source Source connection
     * @param position Initial position
     * @param velocity Initial velocity
     * @param fuse Fuse
     */
    void throw_grenade(connection& source, const glm::vec3& position, const glm::vec3& velocity, float fuse)
    {
        data_stream stream(m_cache_grenade_packet);
        source.fill_grenade_packet(stream, position, velocity, fuse);
        broadcast(source, m_cache_grenade_packet);
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
        broadcast(connection, m_cache_player_left);
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
        for (auto& connection : m_connections) {
            if (connection.is_connected()) {
                connection.send_packet(m_cache_world_update.data(), m_cache_world_update.size(), true, 0);
            }
        }
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
    virtual bool on_connect(connection& /*connection*/)
    {
        // check banned?
        return true;
    }

    /**
     * @brief On disconnect event
     *
     * @param peer Peer
     */
    virtual void on_disconnect(connection& connection)
    {
    }

    /**
     * @brief Handle packets
     *
     * @param peer Peer
     * @param packet Packet
     */
    virtual void on_receive(connection& connection, data_stream& stream)
    {
        auto type = stream.read_type<packet_type>();
        on_receive_default(connection, type, stream);
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
     * @brief On player hit event (weapon)
     *
     * @param source Source player
     * @param target Target player
     * @param type Hit type
     */
    virtual void on_player_hit(connection& source, connection& target, hit_type type)
    {
        if (!source.m_can_kill) {
            return;
        }

        if (source.m_team == target.m_team) {
            return;
        }

        std::uint8_t damage = 0;
        if (type == hit_type::melee) {
            if (glm::distance(source.m_position, target.m_position) > 5.0) {
                return;
            }
            damage = 50;
        } else {
            damage = get_weapon_damage(source.m_weapon, type);
        }

        if (damage > 0) {
            if (target.m_health > damage) {
                target.m_health -= damage;
                target.send_set_hp(source.m_position, true);
            } else {
                kill_type t_kill;
                if (type == hit_type::head) {
                    t_kill = kill_type::headshot;
                } else if (type == hit_type::melee) {
                    t_kill = kill_type::melee;
                } else {
                    t_kill = kill_type::weapon;
                }
                kill(source, target, t_kill, m_respawn_time);
            }
        }
    }

    /**
     * @brief Grenade throw action
     *
     * @param connection Connection
     * @param position Position
     * @param velocity Velocity
     * @param fuse Fuse time
     */
    virtual void
    on_grenade_throw(connection& connection, const glm::vec3& position, const glm::vec3& velocity, float fuse)
    {
        throw_grenade(connection, position, velocity, fuse);
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
    virtual bool on_send_state(connection& /*connection*/)
    {
        return false;
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
        if (!on_send_state(connection)) {
            std::cout << "[WARNING]: failed to send state data" << std::endl;
        }
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
                    m_map_position  = 0;
                    std::cout << "[  LOG  ]: map start packet sent to "
                              << static_cast<std::uint32_t>(connection.get_id()) << std::endl;
                }

            } else if (m_map_used && m_map_ownership == connection.get_id()) {

                auto diff = m_compressed_map.size() - m_map_position;
                auto size = std::min(diff, std::size_t(8192));

                if (size == 0) {
                    on_map_loading_done(connection);
                    m_map_used = false;
                    std::cout << "[  LOG  ]: map loading done, id: " << static_cast<std::uint32_t>(connection.get_id())
                              << std::endl;
                } else if (connection.send_map_chunk(m_compressed_map.data() + m_map_position, size)) {
                    m_map_position += size;
                    // std::cout << "[  LOG  ]: map chunk sent to " << static_cast<std::uint32_t>(connection.get_id())
                    //           << "(size " << size << "): " << m_map_position << " of " << m_compressed_map.size()
                    //           << std::endl;
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
            // connection.set_state(state_type::disconnected); // or connecting on map change?
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
            if (!connection.is_disconnected()) {
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
        connection.set_peer(peer);
        if (!on_connect(connection)) {
            connection.set_peer(nullptr);
            return;
        }
        m_num_players++;
        peer->data = &connection;
        connection.set_state(state_type::connecting);

        std::cout << "[  LOG  ]: connection from " << std::hex << peer->address.host << ':' << std::dec
                  << peer->address.port << " id: " << static_cast<std::uint32_t>(connection.get_id()) << std::endl;
    }

    /**
     * @brief Try disconnecting peer
     *
     * @param peer Peer to be disconnected
     */
    void try_disconnect(ENetPeer* peer)
    {
        auto& connection = peer_to_connection(peer);

        on_disconnect(connection);

        leave(connection);
        peer->data = nullptr;
        connection.reset_values();
        connection.set_state(state_type::disconnected);
        connection.set_peer(nullptr);
        m_num_players--;

        std::cout << "[  LOG  ]: disconnected: " << std::hex << peer->address.host << ':' << std::dec
                  << peer->address.port << " id: " << static_cast<std::uint32_t>(connection.get_id()) << std::endl;
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

    /**
     * @brief Extract connection from peer
     *
     * @param peer Peer
     * @return Reference to connection
     */
    static connection& peer_to_connection(ENetPeer* peer)
    {
        return *reinterpret_cast<connection*>(peer->data);
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
            if (connection.is_disconnected()) {
                return connection;
            }
        }
        throw std::runtime_error("failed to get next free connection");
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

    std::array<std::uint8_t, 3>   m_cache_input_data;
    std::array<std::uint8_t, 3>   m_cache_weapon_input;
    std::array<std::uint8_t, 5>   m_cache_kill_action;
    std::array<std::uint8_t, 32>  m_cache_create_player;
    std::array<std::uint8_t, 769> m_cache_world_update;
    std::array<std::uint8_t, 90>  m_cache_state_data;
    std::array<std::uint8_t, 2>   m_cache_player_left;
    std::array<std::uint8_t, 30>  m_cache_grenade_packet;
    std::vector<char>             m_compressed_map;
    std::size_t                   m_map_position;
    bool                          m_map_used{false};
    std::uint8_t                  m_map_ownership;

    std::vector<connection> m_connections; //!< Player connections

    std::mt19937                                       m_generator;          //!< Random number generator
    std::uniform_real_distribution<float>              m_distribution;       //!< Real number distribution
    std::chrono::time_point<std::chrono::steady_clock> m_respawn_timer;      //!< Respawn timer
    std::chrono::time_point<std::chrono::steady_clock> m_world_update_timer; //!< World update timer
};

} // namespace spadesx
