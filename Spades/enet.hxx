/**
 *
 * SpadesX
 *
 */

#pragma once

#include <cstdint>
#include <enet/enet.h>
#include <iostream>
#include <stdexcept>

namespace spadesx {

class enet
{
  public:
    static enet& get()
    {
        static enet s_enet;
        return s_enet;
    }

    /**
     * @brief Create host
     *
     * @param port Port
     * @param connections Max number of connections
     * @param channels Max number of channels
     * @param bandwidth_in Max input bandwidth (0 = no limit)
     * @param bandwidth_out Max output bandwidth (0 = no limit)
     * @return ENet host
     */
    [[nodiscard]] ENetHost* host(std::uint16_t port,
                                 std::uint8_t  connections,
                                 std::uint8_t  channels,
                                 std::uint32_t bandwidth_in,
                                 std::uint32_t bandwidth_out) const
    {
        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = port;

        ENetHost* _host = enet_host_create(&address, connections, channels, bandwidth_in, bandwidth_out);

        if (_host == nullptr) {
            throw std::runtime_error("failed to create host");
        }

        if (enet_host_compress_with_range_coder(_host) != 0) {
            throw std::runtime_error("failed to set compress with range coder");
        }

        return _host;
    }

    /**
     * @brief Destroy host
     *
     * @param _host Host
     */
    void destroy(ENetHost* _host) const
    {
        enet_host_destroy(_host);
    }

  private:
    bool m_is_initialized{false};

    enet()
    {
        if (enet_initialize() != 0) {
            throw std::runtime_error("failed to initialize ENet");
        }
        std::cout << "initialize ENet" << std::endl;
    }

    ~enet()
    {
        enet_deinitialize();
        std::cout << "deinitialize ENet" << std::endl;
    }
};

} // namespace spadesx
