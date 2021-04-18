/**
 *
 * SpadesX
 *
 */

#pragma once

#include "base.hxx"
#include "command.hxx"
#include "data/enums.hxx"
#include "handler.hxx"

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
class base_protocol : public server_handler, public command_manager
{
  public:
    /**
     * @brief Construct a new base_protocol object
     *
     */
    base_protocol() : base_protocol{32}
    {
    }

    /**
     * @brief Construct a new base protocol object
     *
     * @param max_players Max number of players
     */
    base_protocol(std::uint8_t max_players) :
        server_handler{max_players},
        m_generator{std::random_device()()},
        m_distribution{0.F, 1.F}
    {
    }

    ~base_protocol() override = default;

    /**
     * @brief Check restock
     *
     * @param source Source connection
     * @param base Base
     * @return true If restock is possible (distance and time, not team)
     */
    bool check_restock(connection& source, base_data& base) const
    {
        return source.m_restock_time == 0 && base.distance(source) <= m_base_trigger_distance;
    }

    /**
     * @brief Restock and broadcast
     *
     * @param source Source connection
     * @param restock_time Restock time
     */
    void restock(connection& source, std::uint8_t restock_time)
    {
        source.m_reserve_ammo = get_weapon_stock(source.weapon());
        source.m_restock_time = restock_time;
        source.m_health       = 100;
        broadcast_restock(source);
    }

    /**
     * @brief Get spawn location
     *
     */
    virtual void get_spawn_location(team_type team, entity& entity)
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
        default_receive(connection, type, stream);
    }

    void on_message(connection& source, chat_type type, std::string_view message) override
    {
        if (!message.empty()) {
            if (message[0] == '/') {
                if (!execute_command(message, source, *this)) {
                    system_message(source, "invalid command");
                }
            } else {
                broadcast_message(source, type, message);
            }
        }
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
                if (other.m_has_joined) {
                    connection.send_existing_player(other);
                }
            }
        }
        for (auto& other : m_connections) {
            if (connection != other && other.is_connected() && !other.m_alive) {
                if (other.m_has_joined) {
                    connection.send_kill_action(other);
                }
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
                    m_map_ownership = connection.id();
                    m_map_position  = 0;
                    std::cout << "[  LOG  ]: map start packet sent to " << static_cast<std::uint32_t>(connection.id())
                              << std::endl;
                }

            } else if (m_map_used && m_map_ownership == connection.id()) {

                auto diff = m_compressed_map.size() - m_map_position;
                auto size = std::min(diff, std::size_t(8192));

                if (size == 0) {
                    on_map_loading_done(connection);
                    m_map_used = false;
                    std::cout << "[  LOG  ]: map loading done, id: " << static_cast<std::uint32_t>(connection.id())
                              << std::endl;
                } else if (connection.send_map_chunk(m_compressed_map.data() + m_map_position, size)) {
                    m_map_position += size;
                    // std::cout << "[  LOG  ]: map chunk sent to " << static_cast<std::uint32_t>(connection.id())
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
        if (std::chrono::duration<double>(now - m_one_second_timer).count() >= 1) {
            m_one_second_timer = now;
            for (auto& connection : m_connections) {
                if (connection.m_respawn_time != 0) {
                    connection.m_respawn_time--;
                }
                if (connection.m_restock_time != 0) {
                    connection.m_restock_time--;
                }
            }
        }

        double delta = std::chrono::duration<double>(now - m_world_update_timer).count();
        if (delta >= m_world_update_delta) {
            m_world_update_timer = now;
            // update world
            // ...
            world_update(static_cast<float>(delta)); // or m_world_update_delta?
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
            if (m_map_used && m_map_ownership == connection.id()) {
                m_map_used = false;
            }

            if (connection.m_has_joined) {
                on_disconnect(connection);
            }
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
    double m_world_update_delta{0.1};

    float        m_base_trigger_distance{5.F};
    std::uint8_t m_restock_time{15};

    std::vector<char> m_compressed_map;
    std::size_t       m_map_position;
    bool              m_map_used{false};
    std::uint8_t      m_map_ownership;

    std::mt19937                                       m_generator;          //!< Random number generator
    std::uniform_real_distribution<float>              m_distribution;       //!< Real number distribution
    std::chrono::time_point<std::chrono::steady_clock> m_one_second_timer;   //!< Respawn timer
    std::chrono::time_point<std::chrono::steady_clock> m_world_update_timer; //!< World update timer
};

} // namespace spadesx
