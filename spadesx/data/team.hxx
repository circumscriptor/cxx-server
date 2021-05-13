/**
 * @file team.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "color.hxx"
#include "enums.hxx"
#include "name.hxx"
#include "score.hxx"

namespace spadesx {

/**
 * @brief Team data
 *
 */
class team_data : public score_data, public color_data, public name_data<10>
{
  public:
    /**
     * @brief Construct a new team_data object
     *
     * @param team Team
     */
    constexpr team_data(team_type team) noexcept : m_team{team}
    {
    }

    /**
     * @brief Get team type
     *
     * @return Team type
     */
    [[nodiscard]] team_type team() const noexcept
    {
        return m_team;
    }

  private:
    team_type m_team; //!< Team
};

} // namespace spadesx
