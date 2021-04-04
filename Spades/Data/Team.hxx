/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/Types.hxx"
#include "Core/Vector.hxx"

namespace Spades {

/**
 * @brief Team data
 *
 */
struct Team
{
    uint8    mScore{0};    //!< Score (intel captures)
    uint32   mKills{0};    //!< Total kills
    Color3b  mColor;       //!< Team color
    Vector3f mBase;        //!< Base location
    Vector3f mIntel;       //!< Intel location
    char     mName[11];    //!< Name
    bool     mIntelTaken;  //!< Intel taken staus
    uint8    mIntelHolder; //!< Intel holder (enemy team)
};

} // namespace Spades
