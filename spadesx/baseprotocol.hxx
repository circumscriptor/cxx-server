/**
 *
 * SpadesX
 *
 */

#pragma once

#include "data/enums.hxx"
#include "world.hxx"

#include <chrono>
#include <glm/glm.hpp>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>

namespace spadesx {

/**
 * @brief Protocol base
 *
 */
class base_protocol : public world_manager, public command_manager
{
  public:
    /**
     * @brief Construct a new base protocol object
     *
     * @param max_players Max number of players
     */
    base_protocol(std::uint8_t max_players) :
        world_manager{max_players},
        m_generator{std::random_device()()},
        m_distribution{0.F, 1.F}
    {
    }

    virtual ~base_protocol() = default;

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

                broadcast_input_data(connection);
            } break;
            case packet_type::weapon_input:
            {
                if (connection.get_id() != stream.read_byte()) {
                    std::cout << "[WARNING]: weapon input - invalid id received" << std::endl;
                }

                connection.set_weapon_input(stream.read_byte());

                broadcast_weapon_input(connection);
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
            case packet_type::set_tool:
            {
                if (connection.get_id() != stream.read_byte()) {
                    std::cout << "[WARNING]: set tool - invalid id received" << std::endl;
                }

                connection.m_tool = stream.read_type<tool_type>();

                broadcast_set_tool(connection);
            } break;
            case packet_type::set_color:
            {
                if (connection.get_id() != stream.read_byte()) {
                    std::cout << "[WARNING]: set color - invalid id received" << std::endl;
                }

                stream.read_color3b(connection.m_color);

                broadcast_set_color(connection);
            } break;
            case packet_type::block_action:
            {
                if (connection.get_id() != stream.read_byte()) {
                    std::cout << "[WARNING]: block action - invalid id received" << std::endl;
                }

                auto action = stream.read_type<block_action_type>();
                auto x      = stream.read_int();
                auto y      = stream.read_int();
                auto z      = stream.read_int();

                on_block_action(connection, action, x, y, z);
            } break;
            case packet_type::chat_message:
            {
                if (connection.get_id() != stream.read_byte()) {
                    std::cout << "[WARNING]: block action - invalid id received" << std::endl;
                }

                auto        type = stream.read_type<chat_type>();
                const auto* data = stream.data();
                auto        left = stream.left();
                if (data[left - 1] == 0) {
                    left -= 1;
                }
                std::string_view message(data, left);

                if (!message.empty()) {
                    if (message[0] == '/') {
                        if (!execute_command(message, connection, *this)) {
                            system_message(connection, "invalid command");
                        }
                    } else {
                        broadcast_message(connection, type, message);
                    }
                }
            } break;
            case packet_type::weapon_reload:
            {
                if (connection.get_id() != stream.read_byte()) {
                    std::cout << "[WARNING]: weapon reload - invalid id received" << std::endl;
                }

                connection.m_clip_ammo    = stream.read_byte();
                connection.m_reserve_ammo = stream.read_byte();

                broadcast_weapon_reload(connection);
            } break;
            case packet_type::change_team:
            {
                if (connection.get_id() != stream.read_byte()) {
                    std::cout << "[WARNING]: change team - invalid id received" << std::endl;
                }

                auto team = stream.read_type<team_type>();
                // TODO: check team is valid
                if (connection.m_alive) {
                    kill_and_broadcast(connection, connection, kill_type::team_change, m_respawn_time);
                    connection.m_alive = false;
                }
                connection.m_team = team;
            } break;
            case packet_type::change_weapon:
            {
                if (connection.get_id() != stream.read_byte()) {
                    std::cout << "[WARNING]: change weapon - invalid id received" << std::endl;
                }

                auto weapon = stream.read_type<weapon_type>();
                // TODO: check team is valid
                if (connection.m_alive) {
                    kill_and_broadcast(connection, connection, kill_type::weapon_change, m_respawn_time);
                    connection.m_alive = false;
                }
                connection.m_weapon = weapon;
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

                const auto* data = stream.data();
                auto        left = stream.left();
                if (data[left - 1] == 0) {
                    left = std::strlen(data);
                }
                connection.set_name({data, left});

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
    virtual bool on_connect(ENetPeer* peer)
    {
        // check banned?
        connection& connection = next_free_connection();
        assign_connection(peer, connection);
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
        broadcast_create(connection);
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
        kill_and_broadcast(killer, victim, type, m_respawn_time);
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
                kill_and_broadcast(source, target, t_kill, m_respawn_time);
            }
        }
    }

    /**
     * @brief Grenade throw action
     *
     * @param source Connection
     * @param position Position
     * @param velocity Velocity
     * @param fuse Fuse time
     */
    virtual void on_grenade_throw(connection& source, const glm::vec3& position, const glm::vec3& velocity, float fuse)
    {
        broadcast_grenade(source, position, velocity, fuse);
    }

    /**
     * @brief Block action
     *
     * @param source Connection
     */
    virtual void
    on_block_action(connection& source, block_action_type action, std::uint32_t x, std::uint32_t y, std::uint32_t z)
    {
        if (action == block_action_type::build) {
            m_map->modify_block(x, y, z, true, source.m_color.to_uint32());
        } else if (action == block_action_type::bullet_or_spade) {
            m_map->destroy_block(x, y, z);
        } else if (action == block_action_type::spade_secondary) {
            m_map->destroy_block_secondary(x, y, z);
        } else if (action == block_action_type::grenade) {
            std::cout << "grenade block action: " << x << ' ' << y << ' ' << z << std::endl;
        }

        broadcast_block_action(source, x, y, z, action);
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

        system_message(connection, "Welcome to experimental SpadesX server");
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

        if (on_connect(peer)) {
            std::cout << "[  LOG  ]: connection from " << std::hex << peer->address.host << ':' << std::dec
                      << peer->address.port << std::endl;
            return;
        }
    }

    /**
     * @brief Try disconnecting peer
     *
     * @param peer Peer to be disconnected
     */
    void try_disconnect(ENetPeer* peer)
    {
        if (peer->data != nullptr) {
            auto& connection = peer_to_connection(peer);

            // prevent locking map
            if (m_map_used && m_map_ownership == connection.get_id()) {
                m_map_used = false;
            }

            on_disconnect(connection);
            broadcast_leave(connection);

            detach_connection(connection);
            peer->data = nullptr;
        }

        std::cout << "[  LOG  ]: disconnected: " << std::hex << peer->address.host << ':' << std::dec
                  << peer->address.port << std::endl;
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
     * @brief Get random float number in range 0 to 1
     *
     * @return Float number
     */
    float get_random_float()
    {
        return m_distribution(m_generator);
    }

  protected:
    std::uint8_t m_respawn_time{0}; //!< Current respawn time

    double m_world_update_delta{0.1};

    std::vector<char> m_compressed_map;
    std::size_t       m_map_position;
    bool              m_map_used{false};
    std::uint8_t      m_map_ownership;

    std::mt19937                                       m_generator;          //!< Random number generator
    std::uniform_real_distribution<float>              m_distribution;       //!< Real number distribution
    std::chrono::time_point<std::chrono::steady_clock> m_respawn_timer;      //!< Respawn timer
    std::chrono::time_point<std::chrono::steady_clock> m_world_update_timer; //!< World update timer
};

} // namespace spadesx
