/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Enums.hxx"
#include "Types.hxx"
#include "Vector.hxx"

typedef struct _ENetPeer ENetPeer;

namespace Spades {

class Protocol;

class Player
{
  public:
    /**
     * @brief Construct a new Player object
     *
     */
    Player(Protocol* protocol) : mProtocol{protocol}
    {
    }

    // Delete copy constructor
    Player(const Player&) = delete;

    /**
     * @brief Check whether is connected
     *
     * @return true If connected
     * @return false If disconnected
     */
    operator bool() const noexcept
    {
        return mState != State::Disconnected;
    }

    /**
     * @brief Reset all values
     *
     */
    void Reset() noexcept
    {
        mPosition    = {0.f, 0.f, 0.f};
        mOrientation = {0.f, 0.f, 0.f};
        mState       = State::Disconnected;
        mInput       = 0;
    }

    /**
     * @brief Functino executed on player connection
     *
     * @param peer Player connection
     */
    void OnConnect(ENetPeer* peer);

  private:
    /**
     * @brief Send map data
     *
     */
    void SendMapData();

    /**
     * @brief Send map chunks
     *
     */
    void SendMapChunk();

    /**
     * @brief Send joining data (state data)
     *
     */
    void SendJoiningData();

    uint32    mID;                         //!< Player ID
    Vector3f  mPosition;                   //!< Position
    Vector3f  mOrientation;                //!< Orientation
    State     mState{State::Disconnected}; //!< Player state
    uint8     mInput{0};                   //!< Input keys
    ENetPeer* mPeer{nullptr};              //!< Peer (connection)
    Protocol* mProtocol;                   //!< Common protocol
};

} // namespace Spades
