/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/Types.hxx"
#include "Core/Vector.hxx"

#include <array>

namespace Spades {

/**
 * @brief Team data
 *
 */
struct Team
{
    uint8                mScore{0};          //!< Score (intel captures)
    uint32               mKills{0};          //!< Total kills
    Color3               mColor;             //!< Team color
    Vector3              mBase;              //!< Base location
    Vector3              mIntel;             //!< Intel location
    bool                 mIntelTaken{false}; //!< Intel taken staus
    uint8                mIntelHolder;       //!< Intel holder (enemy team)
    std::array<char, 10> mName;              //!< Name

    /**
     * @brief Reset values
     *
     */
    void Reset()
    {
        mScore      = 0;
        mKills      = 0;
        mIntelTaken = false;
    }
};

} // namespace Spades
