/**
 *
 * SpadesX
 *
 */

#pragma once

#include "types.hxx"

namespace spadesx {

/**
 * @brief Color data
 *
 */
class color_data
{
  public:
    /**
     * @brief Convert from U32
     *
     * @param color U32 color
     */
    void set_color(std::uint32_t color) noexcept
    {
        m_color.from_uint32(color);
    }

    /**
     * @brief Convert to U32
     *
     * @return U32 color
     */
    [[nodiscard]] std::uint32_t get_color() const noexcept
    {
        return m_color.to_uint32();
    }

    color3b m_color; //!< Color data
};

} // namespace spadesx
