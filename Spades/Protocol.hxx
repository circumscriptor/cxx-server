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
#include "Team.hxx"
#include "Types.hxx"
#include "Vector.hxx"

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

    uint8 IntelFlags() const noexcept
    {
        uint8 flags = 0;
        flags |= (mTeams[0].taken) ? 1 : 0;
        flags |= (mTeams[1].taken) ? 2 : 0;
        return flags;
    }

    void SetSpawn(Team::Enum team, const Quad2f& quad)
    {
        switch (team) {
            case Team::A:
                mTeams[0].spawn = quad;
                break;
            case Team::B:
                mTeams[1].spawn = quad;
                break;
            default:
                break;
        }
    }

    void SetColor(Team::Enum team, const Vector3b& color)
    {
        switch (team) {
            case Team::A:
                mTeams[0].color = color;
                break;
            case Team::B:
                mTeams[1].color = color;
                break;
            default:
                break;
        }
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

    void TryConnect(ENetPeer* peer);

    void TryDisconnect(ENetPeer* peer);

    void ProcessInput(Player& player, DataStream& stream);

    void Update(double time);

  private:
    void UpdatePlayer(Player& player);

    Compressor mCompressor;

    uint8   mNumPlayers{0};  //!< Current number of players
    uint8   mMaxPlayers{32}; //!< Max number of players
    Player* mPlayers;

    Mode mGameMode{Mode::CTF};

    Vector3b mFogColor{0xff, 0xff, 0xff};
    Team     mTeams[2];
    uint8    mScoreLimit;

    std::chrono::time_point<std::chrono::steady_clock> mRespawnTimer;
    std::chrono::time_point<std::chrono::steady_clock> mWorldUpdateTimer;

    DataChunk* mChunks{nullptr};
};

} // namespace Spades
