/**
 *
 * SpadesX
 *
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
    player_state() noexcept = default;

    /**
     * @brief Destroy the player_state object
     *
     */
    ~player_state() noexcept = default;

    /**
     * @brief Reset player's state (on death)
     *
     */
    void reset_player_state_death() noexcept
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
        reset_player_state_death();
        m_can_spawn = false;
        m_can_break = true;
        m_can_kill  = true;
    }

    bool m_alive{false};     //!< Is player alive
    bool m_gliding{false};   //!< In the air (airborne)
    bool m_crouching{false}; //!< Crouching state
    bool m_can_spawn{false}; //!< Is player allowed to spawn
    bool m_can_break{true};  //!< Is player allowed to break blocks
    bool m_can_kill{true};   //!< Is player allowed to kill players
};

} // namespace spadesx
