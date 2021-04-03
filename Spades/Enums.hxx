/**
 *
 * SpadesServer
 *
 */

#pragma once

namespace Spades {

/**
 * @brief Player state
 *
 */
enum class State { Disconnected, StartingMap, LoadingChunks, Joining, Respawning, Spawning, Waiting, Ready };

/**
 * @brief Weapon types
 *
 */
enum class Weapon { Rifle = 0, SMG = 1, Shotgun = 2 };

/**
 * @brief Protocol version
 *
 */
enum class Version { v0_75 = 3, v0_76 = 4 };

/**
 * @brief Disconnect reason
 *
 */
enum class Reason { Banned = 1, IpLimitExceeded = 2, WrongProtocolVersion = 3, ServerFull = 4, Kicked = 10 };

/**
 * @brief Packet type
 *
 */
enum class PacketType {
    PositionData     = 0,
    OrientationData  = 1,
    WorldUpdate      = 2,
    InputData        = 3,
    WeaponInput      = 4,
    HitPacket        = 5,
    SetHP            = 5,
    GrenadePacket    = 6,
    SetTool          = 7,
    SetColor         = 8,
    ExistingPlayer   = 9,
    ShortPlayerData  = 10,
    MoveObject       = 11,
    CreatePlayer     = 12,
    BlockAction      = 13,
    BlockLine        = 14,
    StateData        = 15,
    KillAction       = 16,
    ChatMessage      = 17,
    MapStart         = 18,
    MapChunk         = 19,
    PlayerLeft       = 20,
    TerritoryCapture = 21,
    ProgressBar      = 22,
    IntelCapture     = 23,
    IntelPickup      = 24,
    IntelDrop        = 25,
    Restock          = 26,
    FogColor         = 27,
    WeaponReload     = 28,
    ChangeTeam       = 29,
    ChangeWeapon     = 30,
    MapCached        = 31
};

/**
 * @brief Game mode
 *
 */
enum class Mode { CTF = 0, TC = 1 };

/**
 * @brief Currently held tool
 *
 */
enum class Tool { Spade = 0, Block = 1, Gun = 2, Grenade = 3 };

/**
 * @brief Kill type (used by kill action)
 *
 */
enum class KillType { Weapon = 0, Headshot = 1, Melee = 2, Grenade = 3, Fall = 4, TeamChange = 5, ClassChange = 6 };

} // namespace Spades
