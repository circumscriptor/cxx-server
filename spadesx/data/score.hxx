/**
 *
 * SpadesX
 *
 */

#pragma once

#include <cstdint>

namespace spadesx {

/**
 * @brief Score data
 *
 */
class score_data
{
  public:
    /**
     * @brief Reached score
     *
     * @param score Score
     * @return true If reached score
     */
    [[nodiscard]] bool has_reached_score(std::uint8_t score) const noexcept
    {
        return m_score >= score;
    }

    /**
     * @brief Reset score
     *
     */
    void reset_score() noexcept
    {
        m_score  = 0;
        m_kills  = 0;
        m_deaths = 0;
    }

    /**
     * @brief Add score
     *
     */
    score_data& add_score() noexcept
    {
        m_score += 1;
        return *this;
    }

    /**
     * @brief Add kill score
     *
     */
    void add_kill_score() noexcept
    {
        m_kills += 1;
    }

    /**
     * @brief Add death score
     *
     */
    void add_death_score() noexcept
    {
        m_deaths += 1;
    }

    std::uint8_t  m_score{0};  //!< Score
    std::uint32_t m_kills{0};  //!< Kill score
    std::uint32_t m_deaths{0}; //!< Death counter
};

} // namespace spadesx
