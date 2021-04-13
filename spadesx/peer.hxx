/**
 *
 * SpadesX
 *
 */

#pragma once

#include <cstdint>
#include <enet/enet.h>

namespace spadesx {

class peer
{
  public:
    /**
     * @brief Construct a new peer object
     *
     */
    peer() noexcept = default;

    /**
     * @brief Destroy the peer object
     *
     */
    ~peer()
    {
        if (m_peer != nullptr) {
            enet_peer_disconnect(m_peer, 0);
        }
    }

    /**
     * @brief Check whether peer is valid (not nullptr)
     *
     * @return true If peer is valid
     */
    [[nodiscard]] bool is_valid() const noexcept
    {
        return m_peer != nullptr;
    }

    /**
     * @brief Assign ENetPeer pointer
     *
     * @param peer ENetPeer pointer
     */
    void set_peer(ENetPeer* peer) noexcept
    {
        m_peer = peer;
    }

    /**
     * @brief Get IP address
     *
     * @return Address
     */
    std::uint32_t get_address()
    {
        return m_peer->address.host;
    }

    /**
     * @brief Reset peer (forcefully disconnect)
     *
     */
    void reset() noexcept
    {
        if (is_valid()) {
            enet_peer_reset(m_peer);
            m_peer = nullptr;
        }
    }

    /**
     * @brief Disconnect peer
     *
     * @param reason Reason
     */
    void disconnect(std::uint32_t reason)
    {
        if (is_valid()) {
            enet_peer_disconnect(m_peer, reason);
            m_peer = nullptr;
        }
    }

    /**
     * @brief Send packet
     *
     * @param packet Packet to be sent
     * @param channel Channel to be used
     * @return true Success
     */
    bool send(ENetPacket* packet, std::uint8_t channel = 0)
    {
        if (is_valid()) {
            if (packet != nullptr) {
                return enet_peer_send(m_peer, channel, packet) == 0;
            }
            return false;
        }
        return false;
    }

  private:
    ENetPeer* m_peer{nullptr}; //!< ENetPeer pointer
};

} // namespace spadesx
