/**
 *
 * SpadesServer
 *
 */

#pragma once

#include <cassert>
#include <cmath>

namespace Spades {

///
/// @brief Auxiliary math constants
///
///
struct Constants
{
    static constexpr const float PI      = float(3.14159265358979323846);  //!< Constant PI
    static constexpr const float PI_2    = float(1.57079632679489661923);  //!< Constant PI / 2
    static constexpr const float PI_4    = float(0.78539816339744830961);  //!< Constant PI / 4
    static constexpr const float PI_D180 = float(0.01745329251994329576);  //!< Constant PI / 180
    static constexpr const float PI_180D = float(57.29577951308232087679); //!< Constant 180 / PI
    static constexpr const float EPSILON = float(1e-6);                    //!< Constant epsilon (used by Equal)
};

/**
 * @brief Auxiliary math functions
 *
 */
struct Math
{
    ///
    /// @brief Check whether two values are equal (epsilon)
    ///
    /// @param a The first value
    /// @param b The second value
    /// @return True if equal
    ///
    static inline auto Equal(float a, float b) -> bool
    {
        return fabsf(a - b) <= Constants::EPSILON;
    }

    ///
    /// @brief Change radians to degrees
    ///
    /// @param radians Angle in radians
    /// @return Angle in degrees
    ///
    static constexpr auto Degrees(float radians) noexcept -> float
    {
        return radians * Constants::PI_180D;
    }

    ///
    /// @brief Change degrees to radians
    ///
    /// @param degrees Angle in degrees
    /// @return Angle in radians
    ///
    static constexpr auto Radians(float degrees) noexcept -> float
    {
        return degrees * Constants::PI_D180;
    }
};

} // namespace Spades
