/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/DataStream.hxx"
#include "Core/Types.hxx"
#include "Data/Enums.hxx"

typedef struct _ENetPeer ENetPeer;

namespace Spades {

class Peer
{
  public:
    /**
     * @brief Construct a new Peer object
     *
     */
    Peer() noexcept = default;

    /**
     * @brief Construct a new Peer object
     *
     * @param peer ENetPeer pointer
     */
    Peer(ENetPeer* peer) noexcept : mPeer{peer}
    {
    }

    /**
     * @brief Destroy the Peer object
     *
     */
    ~Peer()
    {
        if (IsValid()) {
            Disconnect(Reason::Unknown);
        }
    }

    /**
     * @brief Assign ENetPeer pointer
     *
     * @param peer ENetPeer pointer
     * @return Reference to this
     */
    Peer& operator=(ENetPeer* peer) noexcept
    {
        mPeer = peer;
        return *this;
    }

    /**
     * @brief Check whether peer is valid (not nullptr)
     *
     * @return true If peer is valid
     */
    bool IsValid() const noexcept
    {
        return mPeer != nullptr;
    }

    /**
     * @brief Reset ENetPeer pointer
     *
     */
    void Reset() noexcept
    {
        mPeer = nullptr;
    }

    /**
     * @brief Disconnect peer
     *
     * @param reason Reason
     */
    void Disconnect(Reason reason);

    /**
     * @brief Send packet
     *
     * @param data Data to be sent
     * @param channel Channel
     * @param unsequenced If true, send unsequenced packet
     * @return true Success
     */
    bool Send(DataStream& data, uint8 channel = 0, bool unsequenced = false);

  private:
    ENetPeer* mPeer{nullptr}; //!< ENetPeer pointer
};

} // namespace Spades
