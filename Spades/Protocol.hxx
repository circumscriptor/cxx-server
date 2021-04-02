/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Enums.hxx"
#include "Player.hxx"
#include "Team.hxx"
#include "Types.hxx"
#include "Vector.hxx"

#include <vector>

namespace Spades {

class Protocol
{
    friend class Player;

  public:
    /**
     * @brief Construct a new Protocol object
     *
     * @param maxPlayers Max number of players
     */
    Protocol(uint8 maxPlayers) : mMaxPlayers{maxPlayers}, mTeams{{"TEAM A", 6}, {"TEAM B", 6}}
    {
    }

    /**
     * @brief Get number of available connections
     *
     * @return Number of available connections
     */
    uint8 Available() const noexcept
    {
        return mMaxPlayers - mNumPlayers;
    }

    uint8 IntelFlags() const noexcept
    {
        uint8 flags = 0;
        flags |= (mTeams[0].taken) ? 1 : 0;
        flags |= (mTeams[1].taken) ? 2 : 0;
        return flags;
    }

  private:
    uint8               mNumPlayers{0};  //!< Current number of players
    uint8               mMaxPlayers{32}; //!< Max number of players
    std::vector<Player> mPlayers;        //!< Players

    Vector3b mFogColor;
    Team     mTeams[2];
    uint8    mScoreLimit;
};

} // namespace Spades
