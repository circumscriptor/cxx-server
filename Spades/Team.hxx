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
  public:
    /**
     * @brief Team enum
     *
     */
    enum TeamE {
        SPECTATOR = 0xFF,
        A         = 0,
        B         = 1,
    };

    uint8    score{0}; //!< score (intel captures)
    uint32   kills{0}; //!< total kills
    Vector3b color;    //!< team color
    Vector3f base;     //!< base location
    Vector3f intel;    //!< intel location
    char     name[11]; //!< name
    bool     taken;    //!< intel taken
    uint8    holder;   //!< intel holder (enemy team)
    Quad2f   spawn;    //!< spanw area

    /**
     * @brief Construct a new Team object
     *
     */
    Team(const char* src, uint8 length)
    {
        if (length > 10) {
            length = 10;
        }
        for (uint8 i = 0; i < length; ++i) {
            name[i] = src[i];
        }
        name[length] = 0;
    }
};

} // namespace Spades
