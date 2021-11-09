/**
 * @file team.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "name.hxx"
#include "score.hxx"

namespace spadesx {

/**
 * @brief Team data
 *
 */
class team_data : public name_data<10>
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

    /**
     * @brief Get reference to score data
     *
     * @return Reference to score data
     */
    color3b& color() noexcept
    {
        return m_color;
    }

    /**
     * @brief Get reference to score data
     *
     * @return Reference to score data
     */
    [[nodiscard]] const color3b& color() const noexcept
    {
        return m_color;
    }

    /**
     * @brief Get reference to score data
     *
     * @return Reference to score data
     */
    score_data& score() noexcept
    {
        return m_score;
    }

    /**
     * @brief Get reference to score data
     *
     * @return Reference to score data
     */
    [[nodiscard]] const score_data& score() const noexcept
    {
        return m_score;
    }

  private:
    color3b    m_color; //!< Team color
    score_data m_score; //!< Score
    team_type  m_team;  //!< Team
};

} // namespace spadesx
