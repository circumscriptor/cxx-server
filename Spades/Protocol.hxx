/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Connection.hxx"
#include "Core/DataStream.hxx"
#include "Core/Types.hxx"
#include "Core/Vector.hxx"
#include "Data/Enums.hxx"
#include "Data/Player.hxx"
#include "Data/Team.hxx"
#include "Util/Compress.hxx"
#include "Util/Map.hxx"
#include "Util/Spawn.hxx"

#include <array>
#include <chrono>
#include <vector>

namespace Spades {

class Protocol
{
  public:
    /**
     * @brief Construct a new Protocol object
     *
     */
    Protocol() : Protocol(32)
    {
    }

    /**
     * @brief Construct a new Protocol object
     *
     * @param maxPlayers Max number of players
     */
    Protocol(uint8 maxPlayers);

    /**
     * @brief Destroy the Protocol object
     *
     */
    ~Protocol();

    Map& GetMap()
    {
        return mMap;
    }

    void SetTeamName(uint8 team, const char* name)
    {
        uint8 i;
        for (i = 0; i < 10 && name[i]; ++i) {
            mTeams[team].mName[i] = name[i];
        }
        mTeams[team].mName[i] = 0;
    }

    Team& GetTeam(uint8 i)
    {
        return mTeams[i];
    }

    void SetSpawn(TeamType team, const Spawn& spawn)
    {
        switch (team) {
            case TeamType::A:
                mSpawns[0] = spawn;
                break;
            case TeamType::B:
                mSpawns[1] = spawn;
                break;
            case TeamType::SPECTATOR:
                mSpawns[2] = spawn;
                break;
        }
    }

    void GetSpawnLocation(TeamType team, Vector3& out)
    {
        switch (team) {
            case TeamType::A:
                mSpawns[0].GetLocation(mRandom, mMap, out);
                break;
            case TeamType::B:
                mSpawns[1].GetLocation(mRandom, mMap, out);
                break;
            case TeamType::SPECTATOR:
                mSpawns[2].GetLocation(mRandom, mMap, out);
                break;
        }
    }

    /**
     * @brief Broadcast packet
     *
     * @param data Data to be brodcasted
     * @param includeSender If true include sender
     * @param channel Channel
     */
    void Broadcast(const Connection& sender, DataStream& data, bool includeSender = true, uint8 channel = 0);

    void OnConnect(ENetPeer* peer);

    void OnDisconnect(ENetPeer* peer);

    void Receive(ENetPeer* peer, ENetPacket* packet);

    void Block(Connection& connection, uint32 x, uint32 y, uint32 z, BlockActionType action);

    void Kill(Connection& killer, Connection& victim, KillType type, uint8 respawnTime);

    void Create(Connection& connection);

    void Start();

    void Update();

  private:
    void UpdateConnection(Connection& connection);

    Map                     mMap;                        //!< Map
    std::vector<Connection> mConnections;                //!< Connections
    std::array<Team, 2>     mTeams;                      //!< Teams
    uint8                   mMaxPlayers{0};              //!< Maximal number of players
    uint8                   mNumPlayers{0};              //!< Current number of players
    Color3                  mFogColor{0xff, 0xff, 0xff}; //!< Fog color
    uint8                   mScoreLimit{10};             //!< Scorel limit per team
    Random                  mRandom;
    Spawn                   mSpawns[3];
    uint8                   mRespawnTime{0};

    std::chrono::time_point<std::chrono::steady_clock> mRespawnTimer;
    std::chrono::time_point<std::chrono::steady_clock> mWorldUpdateTimer;

    uint8 IntelFlags() const noexcept
    {
        uint8 flags = 0;
        flags |= (mTeams[0].mIntelTaken) ? 1 : 0;
        flags |= (mTeams[1].mIntelTaken) ? 2 : 0;
        return flags;
    }

    /**
     * @brief Cached packets
     *
     */
    struct Cache
    {
        uint8 mMapStart[5];
        // map chunk, not cached
        uint8 mWorldUpdate[769];
        uint8 mPlayerLeft[2];
        uint8 mGrenade[30];
        uint8 mHit[4];
        uint8 mChatMessage[259];
        uint8 mStateData[104];
        uint8 mCreatePlayer[32];
        uint8 mKillAction[5];
        uint8 mExistingPlayer[28];
        uint8 mBlockAction[15];
    } mCache;
};

} // namespace Spades
