/**
 *
 * SpadesX
 *
 */

#pragma once

#include "enums.hxx"

#include <cstdint>

namespace spadesx {

/**
 * @brief Last kill data
 *
 */
class last_kill_data
{
  public:
    /**
     * @brief Set kill data
     *
     * @param killer Killer
     * @param type Kill type
     */
    void set_kill(std::uint8_t killer, kill_type type) noexcept
    {
        m_last_kill_killer = killer;
        m_last_kill_type   = type;
    }

    std::uint8_t m_last_kill_killer; //!< Last death killer
    kill_type    m_last_kill_type;   //!< Last death type
};

} // namespace spadesx
