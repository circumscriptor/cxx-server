/**
 * @file enums.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

namespace cxxserver {

/**
 * @brief Player state
 *
 */
enum class StateType
{
    DISCONNECTED,
    CONNECTING,
    CONNECTED
};

/**
 * @brief Weapon types
 *
 */
enum class WeaponType
{
    RIFLE   = 0,
    SMG     = 1,
    SHOTGUN = 2
};

/**
 * @brief Game mode
 *
 */
enum class GameModeType
{
    CTF = 0,
    TC  = 1
};

/**
 * @brief Currently held tool
 *
 */
enum class ToolType
{
    SPADE   = 0,
    BLOCK   = 1,
    GUN     = 2,
    GRENADE = 3
};

/**
 * @brief Kill type (used by kill action)
 *
 */
enum class KillType
{
    WEAPON        = 0,
    HEADSHOT      = 1,
    MELEE         = 2,
    GRENADE       = 3,
    FALL          = 4,
    TEAM_CHANGE   = 5,
    WEAPON_CHANGE = 6
};

/**
 * @brief Hit type (used by hit event)
 *
 */
enum class HitType
{
    TORSO = 0,
    HEAD  = 1,
    ARMS  = 2,
    LEGS  = 3,
    MELEE = 4
};

/**
 * @brief Chat type
 *
 */
enum class ChatType
{
    ALL    = 0,
    TEAM   = 1,
    SYSTEM = 2
};

/**
 * @brief Block action type
 *
 */
enum class BlockActionType
{
    BUILD           = 0,
    BULLET_OR_SPADE = 1,
    SPADE_SECONDARY = 2,
    GRENADE         = 3
};

/**
 * @brief Team enum
 *
 */
enum class TeamType
{
    SPECTATOR = 0xFF,
    A         = 0,
    B         = 1,
    NEUTRAL   = 2
};

/**
 * @brief Entity type
 *
 */
enum class EntityType
{
    PLAYER,
    BASE,
    INTEL,
    GRENADE
};

} // namespace cxxserver
