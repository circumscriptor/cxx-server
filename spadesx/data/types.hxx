/**
 *
 * SpadesX
 *
 */

#pragma once

#include <cstdint>

namespace spadesx {

/**
 * @brief RGB color
 *
 */
class color3b
{
  public:
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

/**
 * @brief Convert Color3 to ARGB 32bit format
 *
 * @param color Color3 object
 * @return Color in ARGB format
 */
constexpr std::uint32_t color3b_to_uint32(const color3b& color) noexcept
{
    std::uint32_t v = 0;
    // v |= 0x00 << 24;
    v |= std::uint32_t(color.r) << 16;
    v |= std::uint32_t(color.g) << 8;
    v |= std::uint32_t(color.b);
    return v;
}

/**
 * @brief Convert color in ARGB format to Color3
 *
 * @param v Color in ARGB 32bit format
 * @param color Color3 object
 */
constexpr void uint32_to_colo3b(std::uint32_t v, color3b& color) noexcept
{
    color.r = std::uint8_t(v >> 16);
    color.g = std::uint8_t(v >> 8);
    color.b = std::uint8_t(v);
}

} // namespace spadesx
