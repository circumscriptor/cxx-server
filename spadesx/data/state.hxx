/**
 * @file state.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

namespace spadesx {

/**
 * @brief Player state
 *
 */
class player_state
{
  public:
    /**
     * @brief Construct a new player_state object
     *
     */
    constexpr player_state() noexcept = default;

    /**
     * @brief Reset player's state (on death)
     *
     */
    constexpr void reset_player_state_death() noexcept
    {
        m_alive     = false;
        m_gliding   = false;
        m_crouching = false;
    }

    /**
     * @brief Reset player's state
     *
     */
    void reset_player_state() noexcept
    {
        m_muted      = false;
        m_deaf       = false;
        m_alive      = false;
        m_wade       = false;
        m_gliding    = false;
        m_crouching  = false;
        m_jumping    = false;
        m_has_joined = false;
        m_can_spawn  = false;
        m_can_build  = true;
        m_can_kill   = true;
    }

    bool m_deaf{false};       //!< Chat (deaf)
    bool m_muted{false};      //!< Chat (muted)
    bool m_alive{false};      //!< Is player alive
    bool m_wade{false};       //!< Is player in water
    bool m_gliding{false};    //!< In the air (airborne)
    bool m_crouching{false};  //!< Crouching state
    bool m_jumping{false};    //!< Is mid jump?
    bool m_has_joined{false}; //!< Has player joined
    bool m_can_spawn{false};  //!< Is player allowed to spawn
    bool m_can_build{true};   //!< Is player allowed to break blocks
    bool m_can_kill{true};    //!< Is player allowed to kill players
};

} // namespace spadesx
