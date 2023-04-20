///
/// @file Protocol.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#include "Protocol.hpp"

#include "Details/V75/Protocol.hpp"

#include <cstdint>
#include <enet/enet.h>

namespace cxxserver {

Protocol::~Protocol() = default;

Protocol * Protocol::Create(Version version)
{
    if (version == Version::V75)
    {
        return new Details::V75::Protocol;
    }
    return nullptr;
}

} // namespace cxxserver
