/**
 * @file baseconnection.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "peer.hxx"

namespace spadesx {

/**
 * @brief Base connection
 *
 */
class base_connection : public peer
{
  public:
    /**
     * @brief Construct a new base_connection object
     *
     */
    constexpr base_connection() noexcept = default;

    /**
     * @brief Send map start packet
     *
     * @param size Size of the map
     * @param channel Channel
     * @return true On success
     */
    bool send_map_start(std::uint32_t size, std::uint8_t channel = 0)
    {
        ENetPacket* packet = enet_packet_create(nullptr, 5, ENET_PACKET_FLAG_RELIABLE);
        data_stream stream = packet;
        stream.write_type(packet_type::map_start);
        stream.write_int(size);
        return send(packet, channel);
    }

    /**
     * @brief Send map chunk
     *
     * @param chunk Pointer to the memory location containing chunk
     * @param size Size of the chunk
     * @param unsequenced Unsequenced flag
     * @param channel Channel
     * @return true On success
     */
    bool send_map_chunk(void* chunk, std::uint32_t size, bool unsequenced = false, std::uint8_t channel = 0)
    {
        ENetPacket* packet = enet_packet_create(nullptr, size + 1, unsequenced_to_flag(unsequenced));
        data_stream stream = packet;
        stream.write_type(packet_type::map_chunk);
        stream.write_array(chunk, size);
        return send(packet, channel);
    }

    /**
     * @brief Send generic packet
     *
     * @param data Pointer to the memory location containing packet data
     * @param size Size of the packet
     * @param unsequenced Unsequenced flag
     * @param channel Channel
     * @return true On success
     */
    bool send_packet(const void* data, std::size_t size, bool unsequenced = true, std::uint8_t channel = 0)
    {
        ENetPacket* packet = enet_packet_create(data, size, unsequenced_to_flag(unsequenced));
        return send(packet, channel);
    }

    /**
     * @brief Send generic packet
     *
     * @tparam N Size of the packet
     * @param data Pointer to the memory location containing packet data
     * @param unsequenced Unsequenced flag
     * @param channel Channel
     * @return true On success
     */
    template<std::size_t N>
    bool send_packet(const std::array<std::uint8_t, N>& data, bool unsequenced = true, std::uint8_t channel = 0)
    {
        return send_packet(data.data(), data.size(), unsequenced, channel);
    }

    /**
     * @brief Disconnect
     *
     * @param reason Disconnection reason
     */
    void disconnect(reason_type reason)
    {
        peer::disconnect(static_cast<std::uint32_t>(reason));
        set_state(state_type::disconnected);
    }

    /**
     * @brief Check whether connection state is disconnected
     *
     * @return true If state is disconnected
     */
    [[nodiscard]] constexpr bool is_disconnected() const noexcept
    {
        return m_state == state_type::disconnected;
    }

    /**
     * @brief Check whether connection state is connecting
     *
     * @return true If state is connecting
     */
    [[nodiscard]] constexpr bool is_connecting() const noexcept
    {
        return m_state == state_type::connecting;
    }

    /**
     * @brief Check whether connection state is connected
     *
     * @return true If state is connected
     */
    [[nodiscard]] constexpr bool is_connected() const noexcept
    {
        return m_state == state_type::connected;
    }

    /**
     * @brief Set connection state
     *
     * @param state Connection state
     */
    constexpr void set_state(state_type state) noexcept
    {
        m_state = state;
    }

  protected:
    /**
     * @brief Convert bool to ENet packet flag
     *
     * @param unsequenced Unsequenced flag
     * @return ENet packet flag
     */
    static std::uint32_t unsequenced_to_flag(bool unsequenced)
    {
        return unsequenced ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED;
    }

    state_type m_state{state_type::disconnected}; //!< Connection state (used by protocol)
};

} // namespace spadesx
