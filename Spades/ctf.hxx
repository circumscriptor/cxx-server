/**
 *
 * SpadesX
 *
 */

#pragma once

#include "baseprotocol.hxx"
#include "spawn.hxx"

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

class ctf_protocol : public base_protocol
{
  public:
    ctf_protocol(std::uint8_t max_players) : base_protocol(max_players)
    {
    }

    void get_spawn_location(team_type team, entity_type /*entity*/, glm::vec3& v) override
    {
        get_spawn(team).get(get_random_float(), get_random_float(), v);
        v.z = m_map->get_height(v.x, v.y);
    }

    void on_start() override
    {
        get_spawn_location(team_type::a, entity_type::base, m_teams[0].m_base);
        get_spawn_location(team_type::b, entity_type::base, m_teams[1].m_base);
        get_spawn_location(team_type::a, entity_type::intel, m_teams[0].m_intel);
        get_spawn_location(team_type::b, entity_type::intel, m_teams[1].m_intel);
    }

    // bool on_connect(ENetPeer* peer) override
    // {
    //     return true;
    // }

    // void on_disconnect(ENetPeer* peer) override
    // {
    // }

    // void on_receive(ENetPeer* peer, ENetPacket* packet) override
    // {
    // }

    void on_create(connection& connection) override
    {
        get_spawn_location(connection.m_team, entity_type::player, connection.m_position);
        create(connection);
    }

    // void on_kill(connection& killer, connection& victim, kill_type type) override
    // {
    //     kill(killer, victim, type, m_respawn_time);
    // }

    bool send_state(connection& connection)
    {
        data_stream packet(m_cache_state_data);
        packet.write_type(packet_type::state_data);
        packet.write_byte(connection.get_id());
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
        return connection.send_packet(m_cache_state_data.data(), m_cache_state_data.size());
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

  private:
    std::uint8_t             m_score_limit{10};
    std::array<spawn, 3>     m_spawns;
    std::array<team_data, 2> m_teams;
};

} // namespace spadesx
