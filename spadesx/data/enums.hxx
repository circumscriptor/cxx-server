/**
 *
 * SpadesX
 *
 */

#pragma once

namespace spadesx {

/**
 * @brief Player state
 *
 */
enum class state_type { disconnected, connecting, connected };

/**
 * @brief Weapon types
 *
 */
enum class weapon_type { rifle = 0, smg = 1, shotgun = 2 };

/**
 * @brief Protocol version
 *
 */
enum class version_type { v75 = 3, v76 = 4 };

/**
 * @brief Disconnect reason
 *
 */
enum class reason_type {
    unknown                = 0,
    banned                 = 1,
    ip_limit_exceeded      = 2,
    wrong_protocol_version = 3,
    server_full            = 4,
    kicked                 = 10
};

/**
 * @brief Packet type
 *
 */
enum class packet_type {
    position_data     = 0,
    orientation_data  = 1,
    world_update      = 2,
    input_data        = 3,
    weapon_input      = 4,
    hit_packet        = 5,
    set_hp            = 5,
    grenade_packet    = 6,
    set_tool          = 7,
    set_color         = 8,
    existing_player   = 9,
    short_player_data = 10,
    move_object       = 11,
    create_player     = 12,
    block_action      = 13,
    block_line        = 14,
    state_data        = 15,
    kill_action       = 16,
    chat_message      = 17,
    map_start         = 18,
    map_chunk         = 19,
    player_left       = 20,
    territory_capture = 21,
    progress_bar      = 22,
    intel_capture     = 23,
    intel_pickup      = 24,
    intel_drop        = 25,
    restock           = 26,
    fog_color         = 27,
    weapon_reload     = 28,
    change_team       = 29,
    change_weapon     = 30,
    map_cached        = 31
};

/**
 * @brief Game mode
 *
 */
enum class mode_type { ctf = 0, tc = 1 };

/**
 * @brief Currently held tool
 *
 */
enum class tool_type { spade = 0, block = 1, gun = 2, grenade = 3 };

/**
 * @brief Kill type (used by kill action)
 *
 */
enum class kill_type { weapon = 0, headshot = 1, melee = 2, grenade = 3, fall = 4, team_change = 5, weapon_change = 6 };

/**
 * @brief Hit type (used by hit event)
 *
 */
enum class hit_type { torso = 0, head = 1, arms = 2, legs = 3, melee = 4 };

/**
 * @brief Chat type
 *
 */
enum class chat_type { all = 0, team = 1, system = 2 };

/**
 * @brief Block action type
 *
 */
enum class block_action_type { build = 0, bullet_or_spade = 1, spade_secondary = 2, grenade = 3 };

/**
 * @brief Team enum
 *
 */
enum class team_type { spectator = 0xFF, a = 0, b = 1, neutral = 2 };

/**
 * @brief Entity type
 *
 */
enum class entity_type { player, base, intel };

} // namespace spadesx
