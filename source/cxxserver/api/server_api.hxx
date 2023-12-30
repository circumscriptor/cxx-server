///
/// @file server_api.hxx
/// @brief Server API
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include <cxxserver/version.hxx>

#include <array>
#include <cstdint>
#include <functional>
#include <string_view>

namespace cxxserver {

///
/// @brief Server API
///
///
class IServerApi {
public:

    ///
    /// @brief Server API options
    ///
    ///
    struct CreateInfo { };

    IServerApi()                             = default;
    IServerApi(const IServerApi&)            = delete;
    IServerApi(IServerApi&&)                 = delete;
    IServerApi& operator=(const IServerApi&) = delete;
    IServerApi& operator=(IServerApi&&)      = delete;
    virtual ~IServerApi()                    = default;

    ///
    /// @brief Run server (blocks)
    ///
    /// @return Return code (0 on success)
    ///
    virtual int run() = 0;

    ///
    /// @brief Register verify handler
    ///
    /// @param packetType Packet type
    /// @param handler Verify handler
    ///
    virtual void register_handler(std::uint8_t packetType, HandlerType handler) = 0;

    ///
    /// @brief Create server API
    ///
    /// @param createInfo Server API options
    /// @return Pointer to server API or nullptr
    ///
    static IServerApi* create(const CreateInfo& createInfo);

    ///
    /// @brief Initialize library
    ///
    ///
    static bool init();

    ///
    /// @brief Deinitialize library
    ///
    ///
    static void stop();

    ///
    /// @brief Get compiled version string
    ///
    /// @return Compiled version string
    ///
    static std::string_view version();
};

} // namespace cxxserver
