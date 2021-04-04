/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Vector.hxx"

namespace Spades {

/**
 * @brief Team data
 *
 */
struct Team
{
    /**
     * @brief Team enum
     *
     */
    enum Enum {
        SPECTATOR = 0xFF,
        A         = 0,
        B         = 1,
    };

    uint8    mScore{0};    //!< score (intel captures)
    uint32   mKills{0};    //!< total kills
    Vector3b mColor;       //!< team color
    Vector3f mBase;        //!< base location
    Vector3f mIntel;       //!< intel location
    char     mName[11];    //!< name
    bool     mIntelTaken;  //!< intel taken
    uint8    mIntelHolder; //!< intel holder (enemy team)
    Quad2f   mSpawn;       //!< spanw area
};

} // namespace Spades
