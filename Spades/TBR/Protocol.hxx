/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Compress.hxx"
#include "DataStream.hxx"
#include "Enums.hxx"
#include "Packet.hxx"
#include "Player.hxx"
#include "Team.hxx"
#include "Types.hxx"
#include "Vector.hxx"

#include <array>
#include <chrono>

namespace Spades {

class Player;

class Protocol
{
    friend class Player;

  public:
    /**
     * @brief Construct a new Protocol object
     *
     * @param maxPlayers Max number of players
     */
    Protocol(uint8 maxPlayers);

    ~Protocol();

    /**
     * @brief Get number of available connections
     *
     * @return Number of available connections
     */
    uint8 Available() const noexcept
    {
        return mMaxPlayers - mNumPlayers;
    }

    Vector3f GetSpawnLocation(Team::Enum team);

    void Setup();

    void LoadMap(const char* path);

    void BroadcastWorldUpdate();

    void NotifyCreatePlayer(const Player& player);

    void NotifyPlayerLeft(const Player& player);

    void NotifyPlayerInput(const Player& player);

    void NotifyWeaponInput(const Player& player);

    void NotifySetTool(const Player& player);

    void NotifySetColor(const Player& player);

    void NotifyWeaponReload(const Player& player);

    void NotifyKillAction(const Player& player, const Player& target, KillType type, uint8 respawnTime);

    void NotifyGrenade(const GrenadeEvent& event);

    void NotifyChatMessage(const Player& player, const ChatEvent& event);

    void TryConnect(ENetPeer* peer);

    void TryDisconnect(ENetPeer* peer);

    void ProcessInput(Player& player, DataStream& stream);

    void Update(double time);

  private:
    void UpdatePlayer(Player& player);

    Compressor mCompressor;

    uint8 mNumPlayers{0};  //!< Current number of players
    uint8 mMaxPlayers{32}; //!< Max number of players

    std::array<Player, 32> mPlayers;
    StateData              mState;

    std::chrono::time_point<std::chrono::steady_clock> mRespawnTimer;
    std::chrono::time_point<std::chrono::steady_clock> mWorldUpdateTimer;

    DataChunk* mChunks{nullptr};
};

} // namespace Spades
