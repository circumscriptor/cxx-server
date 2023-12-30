#pragma once

#include <cstdint>
#include <enet/enet.h>
#include <functional>

namespace cxxserver {

class Protocol;

///
/// @brief Protocol version
///
///
enum class ProtocolVersion { V75 = 3, V76 = 4 };

///
/// @brief Disconnect reason
///
///
enum class DisconnectReason { UNKNOWN = 0, BANNED = 1, IP_LIMIT_EXCEEDED = 2, WRONG_PROTOCOL_VERSION = 3, SERVER_FULL = 4, KICKED = 10 };

///
/// @brief Result of verify handler
///
///
enum class HandlerResult {
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
using HandlerType = std::function<HandlerResult(const class Packet&)>;

///
/// @brief Server (ENetHost wrapper)
///
///
class Server {
public:

    struct CreateInfo {
        static constexpr std::uint16_t DEFAULT_PORT              = 32887;
        static constexpr std::uint8_t  DEFAULT_CONNECTIONS_LIMIT = 32;

        std::uint32_t   bandwidthIncoming{0};                   //!< Incoming bandwidth in bytes per second
        std::uint32_t   bandwidthOutgoing{0};                   //!< Outgoing bandwidth in bytes per second
        std::uint32_t   timeout{0};                             //!< Server receive event timeout in milliseconds
        std::uint16_t   port{DEFAULT_PORT};                     //!< Server port
        std::uint8_t    connections{DEFAULT_CONNECTIONS_LIMIT}; //!< Max server connections
        ProtocolVersion protocol{ProtocolVersion::V75};         //!< Protocol version
    };

    Server(const Server&)            = delete;
    Server(Server&&)                 = delete;
    Server& operator=(const Server&) = delete;
    Server& operator=(Server&&)      = delete;

    ///
    /// @brief Construct a new Server object
    ///
    /// @param config Create information
    ///
    explicit Server(const CreateInfo& config);

    ///
    /// @brief Destroy the Server object
    ///
    ///
    ~Server();

    ///
    /// @brief Run protocol
    ///
    /// @param protocol Protocol
    /// @return 0 on success
    ///
    bool service(Protocol& protocol);

private:

    ENetHost*     m_host;
    std::uint32_t m_timeout;
};

} // namespace cxxserver
