///
/// @file Protocol.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#include "protocol.hxx"

#include "cxxserver/server_api.hxx"
#include "details/v75/protocol_v75.hxx"

#include <cassert>

namespace cxxserver {

Protocol * Protocol::create(ProtocolVersion version)
{
    switch (version)
    {
        case ProtocolVersion::V75:
            return new details::ProtocolV75();
        case ProtocolVersion::V76:
            return nullptr;
    }
    assert(false);
}

} // namespace cxxserver
