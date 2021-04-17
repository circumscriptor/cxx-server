/**
 *
 * SpadesX
 *
 */

#pragma once

#include "data/enums.hxx"
#include "datastream.hxx"
#include "line.hxx"
#include "world.hxx"

#include <string_view>

namespace spadesx {

/**
 * @brief Default handler
 *
 */
class server_handler : public world_manager
{
  public:
    /**
     * @brief Construct a new handler object
     *
     */
    server_handler() : server_handler{32}
    {
    }

    /**
     * @brief Construct a new base protocol object
     *
     * @param max_players Max number of players
     */
    server_handler(std::uint8_t max_players) : world_manager{max_players}
    {
    }

    virtual ~server_handler() = default;

    /**
     * @brief Handle position data
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_position_data(connection& source, data_stream& stream)
    {
        glm::vec3 position;
        stream.read_vec3(position);
        on_position_data(source, position);
    }

    /**
     * @brief Handle orientation data
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_orientation_data(connection& source, data_stream& stream)
    {
        glm::vec3 orientation;
        stream.read_vec3(orientation);
        on_orientation_data(source, orientation);
    }

    /**
     * @brief Handle input data
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_input_data(connection& source, data_stream& stream)
    {
        if (source.id() != stream.read_byte()) {
            std::cout << "[WARNING]: input data - invalid id received" << std::endl;
        }

        auto input = stream.read_byte();
        on_input_data(source, input);
    }

    /**
     * @brief Handle weapon input
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_weapon_input(connection& source, data_stream& stream)
    {
        if (source.id() != stream.read_byte()) {
            std::cout << "[WARNING]: weapon input - invalid id received" << std::endl;
        }

        auto input = stream.read_byte();
        on_weapon_input(source, (input & 0x01) != 0, (input & 0x02) != 0);
    }

    /**
     * @brief Handle hit packet
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_hit_packet(connection& source, data_stream& stream)
    {
        auto target = stream.read_byte();
        auto type   = stream.read_type<hit_type>();
        if (target < m_num_players) {
            auto& victim = m_connections[target];
            if (victim.is_connected()) {
                on_packet_hit(source, victim, type);
            } else {
                std::cout << "[WARNING]: hit packet - invalid target" << std::endl;
            }
        } else {
            std::cout << "[WARNING]: hit packet - invalid hit id received" << std::endl;
        }
    }

    /**
     * @brief Handle grenade packet
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_grenade_packet(connection& source, data_stream& stream)
    {
        if (source.id() != stream.read_byte()) {
            std::cout << "[WARNING]: grenade packet - invalid id received" << std::endl;
        }

        float     fuse;
        glm::vec3 position;
        glm::vec3 velocity;

        fuse = stream.read_float();
        stream.read_vec3(position);
        stream.read_vec3(velocity);

        on_grenade_throw(source, position, velocity, fuse);
    }

    /**
     * @brief Handle set tool
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_set_tool(connection& source, data_stream& stream)
    {
        if (source.id() != stream.read_byte()) {
            std::cout << "[WARNING]: set tool - invalid id received" << std::endl;
        }

        auto tool = stream.read_type<tool_type>();
        switch (tool) {
            case tool_type::spade:
            case tool_type::block:
            case tool_type::gun:
            case tool_type::grenade:
                on_set_tool(source, tool);
                return;
            default:
                std::cout << "[WARNING]: set tool - invalid type" << std::endl;
                return;
        };
    }

    /**
     * @brief Handle set color
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_set_color(connection& source, data_stream& stream)
    {
        if (source.id() != stream.read_byte()) {
            std::cout << "[WARNING]: set color - invalid id received" << std::endl;
        }

        color3b color;
        stream.read_color3b(color);
        on_set_color(source, color);
    }

    /**
     * @brief Handle block action
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_block_action(connection& source, data_stream& stream)
    {
        if (source.id() != stream.read_byte()) {
            std::cout << "[WARNING]: block action - invalid id received" << std::endl;
        }

        auto action = stream.read_type<block_action_type>();
        auto x      = stream.read_int();
        auto y      = stream.read_int();
        auto z      = stream.read_int();

        if (x >= map::size_x || y >= map::size_y || z >= map::size_z) {
            return;
        }

        on_block_action(source, action, x, y, z);
    }

    /**
     * @brief Handle block line
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_block_line(connection& source, data_stream& stream)
    {
        if (source.id() != stream.read_byte()) {
            std::cout << "[WARNING]: block line - invalid id received" << std::endl;
        }

        glm::uvec3 start;
        glm::uvec3 end;
        stream.read_uvec3(start);
        stream.read_uvec3(end);

        on_block_line(source, start, end);
    }

    /**
     * @brief Handle chat message
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_chat_message(connection& source, data_stream& stream)
    {
        if (source.id() != stream.read_byte()) {
            std::cout << "[WARNING]: chat message - invalid id received" << std::endl;
        }

        auto        type = stream.read_type<chat_type>();
        const auto* data = stream.data();
        auto        left = stream.left();
        if (data[left - 1] == 0) {
            left -= 1;
        }

        switch (type) {
            case chat_type::all:
            case chat_type::team:
            case chat_type::system:
                on_message(source, type, {data, left});
                return;
            default:
                std::cout << "[WARNING]: chat message - invalid type" << std::endl;
                return;
        }
    }

    /**
     * @brief Handle weapon reload
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_weapon_reload(connection& source, data_stream& stream)
    {
        if (source.id() != stream.read_byte()) {
            std::cout << "[WARNING]: weapon reload - invalid id received" << std::endl;
        }

        auto clip    = stream.read_byte();
        auto reserve = stream.read_byte();

        on_weapon_reload(source, clip, reserve);
    }

    /**
     * @brief Handle team change
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_team_change(connection& source, data_stream& stream)
    {
        if (source.id() != stream.read_byte()) {
            std::cout << "[WARNING]: change team - invalid id received" << std::endl;
        }

        auto team = stream.read_type<team_type>();
        switch (team) {
            case team_type::a:
            case team_type::b:
            case team_type::spectator:
                on_team_change(source, team);
                return;
            default:
                std::cout << "[WARNING]: change team - invalid type" << std::endl;
                return;
        }
    }

    /**
     * @brief Handle team change
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_weapon_change(connection& source, data_stream& stream)
    {
        if (source.id() != stream.read_byte()) {
            std::cout << "[WARNING]: change weapon - invalid id received" << std::endl;
        }

        auto weapon = stream.read_type<weapon_type>();
        switch (weapon) {
            case weapon_type::rifle:
            case weapon_type::smg:
            case weapon_type::shotgun:
                on_weapon_change(source, weapon);
                return;
            default:
                std::cout << "[WARNING]: change weapon - invalid type" << std::endl;
                return;
        }
    }

    /**
     * @brief Handle existing player (packet)
     *
     * @param source Source connection
     * @param stream Packet stream
     */
    void handle_existing_player(connection& source, data_stream& stream)
    {
        if (source.id() != stream.read_byte()) {
            std::cout << "[WARNING]: existing player - invalid id received" << std::endl;
        }

        auto    team   = stream.read_type<team_type>();
        auto    weapon = stream.read_type<weapon_type>();
        auto    tool   = stream.read_type<tool_type>();
        auto    kills  = stream.read_int();
        color3b color;
        stream.read_color3b(source.color());

        const auto* data = stream.data();
        auto        left = stream.left();
        for (auto i = 0; i < left; ++i) {
            if (data[i] == 0) {
                left = i;
                break;
            }
        }

        source.m_has_joined = true;

        on_existing_player(source, team, weapon, tool, kills, color, {data, left});
    }

    /**
     * @brief Default on_receive action
     *
     * @param connection Connection
     * @param type Packet type
     * @param strema Packet stream
     */
    void default_receive(connection& connection, packet_type type, data_stream& stream)
    {
        if (!connection.m_has_joined) {
            if (type == packet_type::existing_player) {
                handle_existing_player(connection, stream);
            }
            return;
        }
        switch (type) {
            case packet_type::position_data:
                handle_position_data(connection, stream);
                break;
            case packet_type::orientation_data:
                handle_orientation_data(connection, stream);
                break;
            case packet_type::input_data:
                handle_input_data(connection, stream);
                break;
            case packet_type::weapon_input:
                handle_weapon_input(connection, stream);
                break;
            case packet_type::hit_packet:
                handle_hit_packet(connection, stream);
                break;
            case packet_type::grenade_packet:
                handle_grenade_packet(connection, stream);
                break;
            case packet_type::set_tool:
                handle_set_tool(connection, stream);
                break;
            case packet_type::set_color:
                handle_set_color(connection, stream);
                break;
            case packet_type::block_action:
                handle_block_action(connection, stream);
                break;
            case packet_type::block_line:
                handle_block_line(connection, stream);
                break;
            case packet_type::chat_message:
                handle_chat_message(connection, stream);
                break;
            case packet_type::weapon_reload:
                handle_weapon_reload(connection, stream);
                break;
            case packet_type::change_team:
                handle_team_change(connection, stream);
                break;
            case packet_type::change_weapon:
                handle_weapon_change(connection, stream);
                break;
            case packet_type::existing_player:
                std::cout << "[WARNING]: existing player sent after player has joined" << std::endl;
                handle_existing_player(connection, stream);
                break;
            default:
                std::cout << "[  LOG  ]: unhandled packet: " << static_cast<std::uint32_t>(type) << std::endl;
                break;
        }
    }

    /**
     * @brief On kill event
     *
     * @param killer Killer
     * @param victim Victim
     * @param type Kill type
     * @param respawn_time Respawn time
     */
    virtual void on_kill(connection& killer, connection& victim, kill_type type, std::uint8_t respawn_time)
    {
        kill_and_broadcast(killer, victim, type, respawn_time);
    }

    /**
     * @brief Position update
     *
     * @param source Source connection
     * @param position Position data
     */
    virtual void on_position_data(connection& source, const glm::vec3& position)
    {
        source.m_position = position;
    }

    /**
     * @brief Orientation data
     *
     * @param source Source connection
     * @param orientation Orientation data
     */
    virtual void on_orientation_data(connection& source, const glm::vec3& orientation)
    {
        source.m_orientation = orientation;
    }

    /**
     * @brief Input data
     *
     * @param source Source connection
     * @param input Input data
     */
    virtual void on_input_data(connection& source, std::uint8_t input)
    {
        source.set_input(input);
        broadcast_input_data(source);
    }

    /**
     * @brief Weapon input
     *
     * @param source Source connection
     * @param primary Primary weapon action
     * @param secondary Secondary weapon action
     */
    virtual void on_weapon_input(connection& source, bool primary, bool secondary)
    {
        source.m_primary   = primary;
        source.m_secondary = secondary;
        broadcast_weapon_input(source);
    }

    /**
     * @brief Player hit
     *
     * @param source Source connection
     * @param target Target (connection)
     * @param type Hit type
     */
    virtual void on_packet_hit(connection& source, connection& target, hit_type type)
    {
        if (!source.m_can_kill) {
            return;
        }

        if (source.team() == target.team()) {
            return;
        }

        std::uint8_t damage = 0;
        if (type == hit_type::melee) {
            if (glm::distance(source.m_position, target.m_position) > 5.0) {
                return;
            }
            damage = 50;
        } else {
            damage = source.get_weapon_damage(type);
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
                on_kill(source, target, t_kill, m_respawn_time);
            }
        }
    }

    /**
     * @brief Throw grenade
     *
     * @param source Source connection
     * @param position Initial position
     * @param velocity Initial velocity
     * @param fuse Fuse time
     */
    virtual void on_grenade_throw(connection& source, const glm::vec3& position, const glm::vec3& velocity, float fuse)
    {
        broadcast_grenade(source, position, velocity, fuse);
    }

    /**
     * @brief Set tool
     *
     * @param source Source connection
     * @param tool Tool
     */
    virtual void on_set_tool(connection& source, tool_type tool)
    {
        source.m_tool = tool;
        broadcast_set_tool(source);
    }

    /**
     * @brief Set color
     *
     * @param source Source connection
     * @param color Color
     */
    virtual void on_set_color(connection& source, const color3b& color)
    {
        source.color() = color;
        broadcast_set_color(source);
    }

    /**
     * @brief Chat message
     *
     * @param source Source connection
     * @param type Message type
     * @param message Message
     */
    virtual void on_message(connection& source, chat_type type, std::string_view message)
    {
        if (!message.empty()) {
            broadcast_message(source, type, message);
        }
    }

    /**
     * @brief Block action
     *
     * @param source Source connection
     * @param action Block action
     * @param x The x-coordinate of the block
     * @param y The y-coordinate of the block
     * @param z The z-coordinate of the block
     */
    virtual void
    on_block_action(connection& source, block_action_type action, std::uint32_t x, std::uint32_t y, std::uint32_t z)
    {
        switch (action) {
            case block_action_type::build:
                m_map->modify_block(x, y, z, true, source.get_color());
                break;
            case block_action_type::bullet_or_spade:
                m_map->destroy_block(x, y, z);
                break;
            case block_action_type::spade_secondary:
                m_map->destroy_block_secondary(x, y, z);
                break;
                // TODO: Add grenade
            default:
                return;
        }
        broadcast_block_action(source, x, y, z, action);
    }

    /**
     * @brief Block line
     *
     * @param source Source connection
     * @param start Start position
     * @param end End position
     */
    virtual void on_block_line(connection& source, glm::uvec3 start, glm::uvec3 end)
    {
        if (!source.m_can_build) {
            return;
        }

        if (!m_map->has_neighbor(start.x, start.y, start.z)) {
            return;
        }

        if (!m_map->has_neighbor(end.x, end.y, end.z)) {
            return;
        }

        auto size = m_line.line(start, end);

        for (std::uint32_t i = 0; i < size; ++i) {
            const auto& block = m_line.get(i);
            m_map->modify_block(block.x, block.y, block.z, true, source.get_color());
        }

        broadcast_block_line(source, start, end);
    }

    /**
     * @brief Reload weapon
     *
     * @param source Source connection
     * @param clip Clip ammo
     * @param reserve Reserve ammo
     */
    virtual void on_weapon_reload(connection& source, std::uint8_t clip, std::uint8_t reserve)
    {
        source.m_clip_ammo    = clip;
        source.m_reserve_ammo = reserve;
        broadcast_weapon_reload(source);
    }

    /**
     * @brief Team change
     *
     * @param source Source connection
     * @param team Team
     */
    virtual void on_team_change(connection& source, team_type team)
    {
        if (source.m_alive) {
            on_kill(source, source, kill_type::team_change, m_respawn_time);
        }
        source.change_team(team);
    }

    /**
     * @brief Change weapon
     *
     * @param source Source connection
     * @param weapon Weapon
     */
    virtual void on_weapon_change(connection& source, weapon_type weapon)
    {
        if (source.m_alive) {
            on_kill(source, source, kill_type::weapon_change, m_respawn_time);
        }
        source.set_weapon(weapon);
    }

    /**
     * @brief Existing player packet
     *
     * @param source Source connection
     * @param team Team
     * @param weapon Weapon
     * @param tool Tool (held item)
     * @param kills Number of kills
     * @param color Block color
     * @param name Name
     */
    virtual void on_existing_player(connection& source,
                                    team_type   team,
                                    weapon_type weapon,
                                    tool_type /*tool*/,
                                    std::uint32_t /*kills*/,
                                    const color3b& /*color*/,
                                    std::string_view name)
    {
        if (source.m_alive) {
            std::cout << "[WARNING]: player marked alive has sent existing player packet" << std::endl;
            source.m_alive = false;
        }

        source.change_team(team);
        source.change_weapon(weapon);

        // these must not be provided by player:
        // source.m_tool  = tool;
        // source.m_kills = kills;
        // source.m_color = color;

        source.set_name(name);

        source.m_can_spawn = true;
    }

    /**
     * @brief Set global respawn time
     *
     * @param respawn_time Respawn time
     */
    void set_respawn_time(std::uint8_t respawn_time)
    {
        m_respawn_time = respawn_time;
    }

  protected:
    block_line   m_line;            //!< Block line
    std::uint8_t m_respawn_time{0}; //!< Current respawn time
};

} // namespace spadesx
