/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/DataChunk.hxx"
#include "Data/Enums.hxx"
#include "Data/Team.hxx"
#include "Util/Spawn.hxx"

#include <algorithm>
#include <array>
#include <string>

namespace Spades {

/**
 * @brief Game world
 *
 */
class World : public Map
{
  public:
    /**
     * @brief Set intial values (spawn intel and base), use after map is loaded
     *
     */
    void SetupWorld()
    {
        mTeams[0].Reset();
        mTeams[1].Reset();
        SpawnLocation(TeamType::A, mTeams[0].mBase);
        SpawnLocation(TeamType::B, mTeams[1].mBase);
        SpawnLocation(TeamType::A, mTeams[0].mIntel);
        SpawnLocation(TeamType::B, mTeams[1].mIntel);
    }

    /**
     * @brief Get the Team object
     *
     * @param team Team (A or B, spectator has no Team object)
     * @return Reference to team object
     */
    auto GetTeam(TeamType team) -> Team&
    {
        assert(static_cast<unsigned>(team) < 2);
        return mTeams[static_cast<unsigned>(team)];
    }

    /**
     * @brief Set team name
     *
     * @param team
     * @param name
     */
    void SetTeamName(TeamType team, const std::string& name)
    {
        if (IsValidTeam(team) && team != TeamType::Spectator) {
            auto size = std::min(name.size(), std::size_t(10));
            std::copy(name.begin(), name.begin() + size, GetTeam(team).mName.begin());
        }
    }

    /**
     * @brief Set the team's color
     *
     * @param team Team (A or B, spectator has no color)
     * @param color Color3 object
     */
    void SetTeamColor(TeamType team, const Color3& color)
    {
        if (IsValidTeam(team) && team != TeamType::Spectator) {
            GetTeam(team).mColor = color;
        }
    }

    /**
     * @brief Get team's spawn
     *
     * @param team Team
     * @return Reference to team's spawn
     */
    auto GetSpawn(TeamType team) -> Spawn&
    {
        switch (team) {
            case TeamType::A:
                return mSpawns[0];
            case TeamType::B:
                return mSpawns[1];
            default:
                return mSpawns[2];
        }
    }

    /**
     * @brief Set team's spawn
     *
     * @param team Team
     * @param spawn Spawn object
     */
    void SetSpawn(TeamType team, const Spawn& spawn)
    {
        GetSpawn(team) = spawn;
    }

    /**
     * @brief Get spawn location
     *
     * @param team Team
     * @param position Output position
     */
    void SpawnLocation(TeamType team, Vector3& position)
    {
        GetSpawn(team).GetLocation3(mRandom, *this, position);
    }

    /**
     * @brief Generate intel flags
     *
     * @return Intel flags
     */
    auto IntelFlags() const noexcept -> uint8
    {
        uint8 flags = 0;
        flags |= (mTeams[0].mIntelTaken) ? 1 : 0;
        flags |= (mTeams[1].mIntelTaken) ? 2 : 0;
        return flags;
    }

    /**
     * @brief Check whether the given team is a valid enum
     *
     * @param team Team type enum
     * @return true If the given team is a valid enum
     */
    static constexpr auto IsValidTeam(TeamType team) noexcept -> bool
    {
        switch (team) {
            case TeamType::A:
            case TeamType::B:
            case TeamType::Spectator:
                return true;
            default: // if wrong value was casted to this type
                return false;
        }
    }

  protected:
    std::array<Team, 2>  mTeams;                      //!< Teams
    std::array<Spawn, 3> mSpawns;                     //!< Spawns
    Random               mRandom;                     //!< Random number generator
    uint8                mScoreLimit{10};             //!< Score limit per team
    uint8                mRespawnTime{0};             //!< Respawn time
    Color3               mFogColor{0xff, 0xff, 0xff}; //!< Fog color
};

} // namespace Spades
