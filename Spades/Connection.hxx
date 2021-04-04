/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/DataChunk.hxx"
#include "Core/DataStream.hxx"
#include "Data/Player.hxx"
#include "Util/Peer.hxx"

namespace Spades {

class Connection : public Player, public Peer
{
  public:
    /**
     * @brief Construct a new Connection object
     *
     * @param id Player ID
     */
    Connection(uint8 id) : Player(id)
    {
    }

    // For std::vector
    Connection(const Connection& other) = default;

    /**
     * @brief Destroy the Connection object
     *
     */
    ~Connection() = default;

    /**
     * @brief Check whether player state is disconnected
     *
     * @return true If player state is disconnected
     */
    operator bool() const noexcept
    {
        return mState != State::Disconnected;
    }

    /**
     * @brief Set peer
     *
     * @param peer Peer
     */
    void Connect(ENetPeer* peer) noexcept
    {
        Peer::operator=(peer);
        mState        = State::Connecting;
        mMapStart     = true;
    }

    /**
     * @brief Disconnect
     *
     * @param reason Reason
     */
    void Disconnect(Reason reason)
    {
        Player::Reset();
        Peer::Disconnect(reason);
    }

    /**
     * @brief Reset connection
     *
     */
    void Reset()
    {
        mMapStart = false;
        while (mMapChunk) {
            mMapChunk = mMapChunk->Pop();
        }
        Player::Reset();
        Peer::Reset();
    }

    bool operator==(const Connection& other) const noexcept
    {
        return mID == other.mID;
    }

    bool operator!=(const Connection& other) const noexcept
    {
        return mID != other.mID;
    }

    bool       mMapStart{false};   //!< Started loading map
    DataChunk* mMapChunk{nullptr}; //!< Map chunk (queue)
};

} // namespace Spades
