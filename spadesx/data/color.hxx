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
     * @brief Construct a new color_data object
     *
     */
    constexpr color_data() noexcept = default;

    /**
     * @brief Convert from U32
     *
     * @param color U32 color
     */
    constexpr void set_color(std::uint32_t color) noexcept
    {
        m_color.from_uint32(color);
    }

    /**
     * @brief Convert to U32
     *
     * @return U32 color
     */
    [[nodiscard]] constexpr std::uint32_t get_color() const noexcept
    {
        return m_color.to_uint32();
    }

    /**
     * @brief Get color
     *
     * @return Color
     */
    constexpr color3b& color() noexcept
    {
        return m_color;
    }

    /**
     * @brief Get color
     *
     * @return Color
     */
    [[nodiscard]] constexpr const color3b& color() const noexcept
    {
        return m_color;
    }

  protected:
    color3b m_color; //!< Color data
};

} // namespace spadesx
