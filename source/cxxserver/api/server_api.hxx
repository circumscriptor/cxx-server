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
/// @brief Protocol version
///
///
enum class ProtocolVersion
{
    V75 = 3,
    V76 = 4
};

///
/// @brief Disconnect reason
///
///
enum class DisconnectReason
{
    UNKNOWN                = 0,
    BANNED                 = 1,
    IP_LIMIT_EXCEEDED      = 2,
    WRONG_PROTOCOL_VERSION = 3,
    SERVER_FULL            = 4,
    KICKED                 = 10
};

///
/// @brief Result of verify handler
///
///
enum class HandlerResult
{
    VALID,           //!< Packet is valid
    INVALID_ID,      //!< Packet has invalid source ID
    INVALID_LENGTH,  //!< Packet does not have expected length
    INVALID_CONTENT, //!< Packet content cannot be parsed
    NOT_ALLOWED      //!< Packet operation is not allowed
};

///
/// @brief Handler to verify packet
///
///
using HandlerType = std::function<HandlerResult(const class Packet &)>;

///
/// @brief Server API
///
///
class IServerApi
{
  public:

    ///
    /// @brief Server API options
    ///
    ///
    struct CreateInfo
    {
        static constexpr std::uint16_t DEFAULT_PORT              = 32887;
        static constexpr std::uint8_t  DEFAULT_CONNECTIONS_LIMIT = 32;

        std::uint32_t   bandwidthIncoming { 0 };                   //!< Incoming bandwidth in bytes per second
        std::uint32_t   bandwidthOutgoing { 0 };                   //!< Outgoing bandwidth in bytes per second
        std::uint32_t   timeout { 0 };                             //!< Server receive event timeout in milliseconds
        std::uint16_t   port { DEFAULT_PORT };                     //!< Server port
        std::uint8_t    connections { DEFAULT_CONNECTIONS_LIMIT }; //!< Max server connections
        ProtocolVersion protocol { ProtocolVersion::V75 };         //!< Protocol version
    };

    IServerApi()                               = default;
    IServerApi(const IServerApi &)             = delete;
    IServerApi(IServerApi &&)                  = delete;
    IServerApi & operator=(const IServerApi &) = delete;
    IServerApi & operator=(IServerApi &&)      = delete;
    virtual ~IServerApi()                      = default;

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
    virtual void registerHandler(std::uint8_t packetType, HandlerType handler) = 0;

    ///
    /// @brief Create server API
    ///
    /// @param createInfo Server API options
    /// @return Pointer to server API or nullptr
    ///
    static IServerApi * create(const CreateInfo & createInfo);

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
