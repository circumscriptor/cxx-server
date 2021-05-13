/**
 * @file input.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include <cstdint>

namespace spadesx {

/**
 * @brief Input data
 *
 */
class input_data
{
  public:
    /**
     * @brief Construct a new input_data object
     *
     */
    constexpr input_data() noexcept = default;

    /**
     * @brief Set input
     *
     * @param input Input
     */
    constexpr void set_input(std::uint8_t input) noexcept
    {
        m_up     = ((input & 0x01) != 0);
        m_down   = ((input & 0x02) != 0);
        m_left   = ((input & 0x04) != 0);
        m_right  = ((input & 0x08) != 0);
        m_jump   = ((input & 0x10) != 0);
        m_crouch = ((input & 0x20) != 0);
        m_sneak  = ((input & 0x40) != 0);
        m_sprint = ((input & 0x80) != 0);
    }

    /**
     * @brief Get input data
     *
     * @return Input data
     */
    [[nodiscard]] constexpr std::uint8_t get_input_data() const noexcept
    {
        std::uint8_t input = 0;
        input |= m_up ? 0x01 : 0x00;
        input |= m_down ? 0x02 : 0x00;
        input |= m_left ? 0x04 : 0x00;
        input |= m_right ? 0x08 : 0x00;
        input |= m_jump ? 0x10 : 0x00;
        input |= m_crouch ? 0x20 : 0x00;
        input |= m_sneak ? 0x40 : 0x00;
        input |= m_sprint ? 0x80 : 0x00;
        return input;
    }

    /**
     * @brief Set weapon input
     *
     * @param input Weapon input
     */
    constexpr void set_weapon_input(std::uint8_t input) noexcept
    {
        m_primary   = ((input & 0x01) != 0);
        m_secondary = ((input & 0x02) != 0);
    }

    /**
     * @brief Get weapon input
     *
     * @return Weapon input
     */
    [[nodiscard]] constexpr std::uint8_t get_weapon_input() const noexcept
    {
        std::uint8_t input = 0;
        input |= m_primary ? 0x01 : 0x00;
        input |= m_secondary ? 0x02 : 0x00;
        return input;
    }

    /**
     * @brief Reset input
     *
     */
    constexpr void reset_input_data() noexcept
    {
        m_up        = false;
        m_down      = false;
        m_left      = false;
        m_right     = false;
        m_jump      = false;
        m_crouch    = false;
        m_sneak     = false;
        m_sprint    = false;
        m_primary   = false;
        m_secondary = false;
    }

    bool m_up{false};        //!< Input forward
    bool m_down{false};      //!< Input backward
    bool m_left{false};      //!< Input left
    bool m_right{false};     //!< Input right
    bool m_jump{false};      //!< Input jump
    bool m_crouch{false};    //!< Input crouch
    bool m_sneak{false};     //!< Input sneak
    bool m_sprint{false};    //!< Input sprint
    bool m_primary{false};   //!< Primary action
    bool m_secondary{false}; //!< Secondary action
};

} // namespace spadesx
