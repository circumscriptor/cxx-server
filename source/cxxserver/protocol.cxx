///
/// @file Protocol.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#include "protocol.hxx"

#include <cstdint>

namespace cxxserver {

Protocol * Protocol::create(Version version)
{
    switch (version)
    {
        case Version::V75:
        case Version::V76:
        default:
            return nullptr;
    }
}

} // namespace cxxserver
