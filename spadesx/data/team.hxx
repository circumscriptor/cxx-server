/**
 *
 * SpadesX
 *
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
class team_data : public score_data, public name_data<10>, public color_data
{
  public:
    /**
     * @brief Construct a new team_data object
     *
     * @param team Team
     */
    team_data(team_type team) : m_team{team}
    {
    }

    /**
     * @brief Get team type
     *
     * @return Team type
     */
    [[nodiscard]] team_type type() const noexcept
    {
        return m_team;
    }

  private:
    team_type m_team; //!< Team
};

} // namespace spadesx
