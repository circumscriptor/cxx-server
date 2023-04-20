///
/// @file Protocol.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#include "protocol.hxx"

#include "details/v75/protocol_v75.hxx"

#include <cassert>

namespace cxxserver {

Protocol * Protocol::create(Version version)
{
    switch (version)
    {
        case Version::V75:
            return new details::ProtocolV75();
        case Version::V76:
            return nullptr;
    }
    assert(false);
}

} // namespace cxxserver
