/**
 * @file score.hxx
 * @brief This file is part of the experimental SpadesX project
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
     * @brief Construct a new score data object
     *
     */
    constexpr score_data() = default;

    /**
     * @brief Reached score
     *
     * @param score Score
     * @return true If reached score
     */
    [[nodiscard]] constexpr bool has_reached(std::uint8_t score) const noexcept
    {
        return m_score >= score;
    }

    /**
     * @brief Reset score
     *
     */
    constexpr void reset() noexcept
    {
        m_score  = 0;
        m_kills  = 0;
        m_deaths = 0;
    }

    /**
     * @brief Add score
     *
     */
    constexpr score_data & increase() noexcept
    {
        m_score += 1;
        return *this;
    }

    /**
     * @brief Add kill score
     *
     */
    constexpr score_data & increase_kill() noexcept
    {
        m_kills += 1;
        return *this;
    }

    /**
     * @brief Add death score
     *
     */
    constexpr score_data & increase_death() noexcept
    {
        m_deaths += 1;
        return *this;
    }

    /**
     * @brief Get score
     *
     * @return Score
     */
    [[nodiscard]] constexpr std::uint8_t to_byte() const noexcept
    {
        return m_score;
    }

    /**
     * @brief Get number of kills
     *
     * @return Number of kills
     */
    [[nodiscard]] constexpr std::uint32_t kills() const noexcept
    {
        return m_kills;
    }

    /**
     * @brief Get number of deaths
     *
     * @return Number of deaths
     */
    [[nodiscard]] constexpr std::uint32_t deaths() const noexcept
    {
        return m_deaths;
    }

  protected:

    std::uint8_t  m_score { 0 };  //!< Score
    std::uint32_t m_kills { 0 };  //!< Kill score
    std::uint32_t m_deaths { 0 }; //!< Death counter
};

} // namespace spadesx
