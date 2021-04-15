/**
 *
 * SpadesX
 *
 */

#pragma once

namespace spadesx {

/**
 * @brief Chat data (deaf, muted)
 *
 */
class chat_data
{
  public:
    /**
     * @brief Reset chat data
     *
     */
    void reset_chat()
    {
        m_deaf  = false;
        m_muted = false;
    }

    bool m_deaf{false};
    bool m_muted{false};
};

} // namespace spadesx
