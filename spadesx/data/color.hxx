/**
 * @file color.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "enums.hxx"

#include <cstdint>

namespace spadesx {

/**
 * @brief RGB color
 *
 */
class color3b
{
  public:
    /**
     * @brief Construct a new color3b object
     *
     */
    constexpr color3b() noexcept = default;

    /**
     * @brief Construct a new color3b object
     *
     * @param _r
     * @param _g
     * @param _b
     */
    constexpr color3b(std::uint8_t _r, std::uint8_t _g, std::uint8_t _b) noexcept : r{_r}, g{_g}, b{_b}
    {
    }

    /**
     * @brief Construct a new color3b object
     *
     * @param other Other color3b object to be copied
     */
    constexpr color3b(const color3b& other) noexcept = default;

    /**
     * @brief Copy color3b object
     *
     * @param other Other color3b object to be copied
     * @return Reference to this
     */
    constexpr color3b& operator=(const color3b& other) noexcept = default;

    /**
     * @brief Convert Color3 to ARGB 32bit format
     *
     * @return Color in ARGB format
     */
    [[nodiscard]] constexpr std::uint32_t to_uint() const noexcept
    {
        std::uint32_t v = 0;
        // v |= 0x00 << 24;
        v |= std::uint32_t(r) << 16;
        v |= std::uint32_t(g) << 8;
        v |= std::uint32_t(b);
        return v;
    }

    /**
     * @brief Convert color in ARGB format to Color3
     *
     * @param v Color in ARGB 32bit format
     */
    constexpr void from_uint(std::uint32_t v) noexcept
    {
        r = std::uint8_t(v >> 16);
        g = std::uint8_t(v >> 8);
        b = std::uint8_t(v);
    }

    std::uint8_t r{0}; //!< R-component
    std::uint8_t g{0}; //!< G-component
    std::uint8_t b{0}; //!< B-component
};

} // namespace spadesx
