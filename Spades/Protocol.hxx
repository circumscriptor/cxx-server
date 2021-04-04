/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Connection.hxx"
#include "Core/DataStream.hxx"
#include "Core/Types.hxx"
#include "Data/Enums.hxx"
#include "Data/Team.hxx"
#include "Util/Compress.hxx"

#include <array>
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

    void Update();

  private:
    void UpdateConnection(Connection& connection);

    Compressor              mCompressor;                 //!< Compressor
    std::vector<Connection> mConnections;                //!< Connections
    std::array<Team, 2>     mTeams;                      //!< Teams
    uint8                   mMaxPlayers{0};              //!< Maximal number of players
    uint8                   mNumPlayers{0};              //!< Current number of players
    Color3b                 mFogColor{0xff, 0xff, 0xff}; //!< Fog color
    uint8                   mScoreLimit{10};             //!< Scorel limit per team

    uint8 IntelFlags() const noexcept
    {
        uint8 flags = 0;
        flags |= (mTeams[0].mIntelTaken) ? 1 : 0;
        flags |= (mTeams[1].mIntelTaken) ? 2 : 0;
        return flags;
    }

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
        uint8 mCreatePlayer[23];
        uint8 mExistingPlayer[28];
    } mCache;
};

} // namespace Spades
