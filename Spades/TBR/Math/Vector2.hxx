/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Math.hxx"

namespace Spades {

///
/// @brief 2D Vector
///
///
struct Vector2
{
    float x{0.F}; //!< X coordinate
    float y{0.F}; //!< Y coordinate

    ///
    /// @brief Construct a new Vector 2 object
    ///
    ///
    constexpr Vector2() noexcept = default;

    ///
    /// @brief Construct a new Vector2 object using the same value for each coordinate
    ///
    /// @param s Scalar value (X and Y)
    ///
    constexpr explicit Vector2(float s) noexcept : x(s), y(s)
    {
    }

    ///
    /// @brief Construct a new Vector2 object using two values (one for each coordinate)
    ///
    /// @param a Scalar value (X)
    /// @param b Scalar value (Y)
    ///
    constexpr Vector2(float a, float b) noexcept : x(a), y(b)
    {
    }

    ///
    /// @brief Construct a new Vector2 object with the contents of another Vector2 object
    ///
    /// @param other Vector2 object to be copied
    ///
    constexpr Vector2(const Vector2& other) = default;

    ///
    /// @brief Construct a new Vector2 object using move semantics
    ///
    /// @param other Vector2 object to be moved
    ///
    constexpr Vector2(Vector2&& other) = default;

    ///
    /// @brief Assign the contents of another Vector2 object
    ///
    /// @param other Vector2 object to be copied
    /// @return Reference to this
    ///
    constexpr auto operator=(const Vector2& other) noexcept -> Vector2& = default;

    ///
    /// @brief Assign the contents of another Vector2 object using move semantics
    ///
    /// @param other Vector2 object to be moved
    /// @return Reference to this
    ///
    constexpr auto operator=(Vector2&& other) noexcept -> Vector2& = default;

    ///
    /// @brief Vector addition
    ///
    /// @param other Vector to be added
    /// @return Reference to this
    ///
    constexpr auto operator+=(const Vector2& other) noexcept -> Vector2&
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    ///
    /// @brief Vector subtraction
    ///
    /// @param other Vector to be subtracted
    /// @return Reference to this
    ///
    constexpr auto operator-=(const Vector2& other) noexcept -> Vector2&
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    ///
    /// @brief Multiply each coordinate by scalar
    ///
    /// @param s Scalar used in calculation
    /// @return Reference to this
    ///
    constexpr auto operator*=(float s) noexcept -> Vector2&
    {
        x *= s;
        y *= s;
        return *this;
    }

    ///
    /// @brief Divide each coordinate by scalar
    ///
    /// @param s Scalar used in calculation
    /// @return Reference to this
    ///
    constexpr auto operator/=(float s) noexcept -> Vector2&
    {
        x /= s;
        y /= s;
        return *this;
    }

    ///
    /// @brief Vector addition
    ///
    /// @param other Vector to be added
    /// @return Result of addition
    ///
    constexpr auto operator+(const Vector2& other) const noexcept -> Vector2
    {
        return {x + other.x, y + other.y};
    }

    ///
    /// @brief Vector subtraction
    ///
    /// @param other Vector to be subtracted
    /// @return Result of subtraction
    ///
    constexpr auto operator-(const Vector2& other) const noexcept -> Vector2
    {
        return {x - other.x, y - other.y};
    }

    ///
    /// @brief Multiply each coordinate by scalar
    ///
    /// @param s Scalar used in calculation
    /// @return Result of multiplication
    ///
    constexpr auto operator*(float s) const noexcept -> Vector2
    {
        return {x * s, y * s};
    }

    ///
    /// @brief Divide each coordinate by scalar
    ///
    /// @param s Scalar used in calculation
    /// @return Result of division
    ///
    constexpr auto operator/(float s) const noexcept -> Vector2
    {
        return {x / s, y / s};
    }

    ///
    /// @brief Get opposite vector
    ///
    /// @return Opposite vector
    ///
    constexpr auto operator-() const noexcept -> Vector2
    {
        return {-x, -y};
    }

    ///
    /// @brief Access coordinate by the index
    ///
    /// @param index The index of the coordinate
    /// @return Reference to the required coordinate
    ///
    auto operator[](unsigned index) noexcept -> float&
    {
        assert(index < 2);
        return reinterpret_cast<float*>(this)[index];
    }

    ///
    /// @brief Access coordinate by the index
    ///
    /// @param index The index of the coordinate
    /// @return Value of the required coordinate
    ///
    auto operator[](unsigned index) const noexcept -> const float&
    {
        assert(index < 2);
        return reinterpret_cast<const float*>(this)[index];
    }

    ///
    /// @brief Check whether two vectors are equal
    ///
    /// @param other Vector to be compared
    /// @return True if equal
    ///
    constexpr auto operator==(const Vector2& other) const noexcept -> bool
    {
        return (x == other.x) && (y == other.y);
    }

    ///
    /// @brief Check whether two vectors are not equal
    ///
    /// @param other Vector to be compared
    /// @return False if equal
    ///
    constexpr auto operator!=(const Vector2& other) const noexcept -> bool
    {
        return (x != other.x) || (y != other.y);
    }

    ///
    /// @brief Check whether two vectors are equal (using epsilon)
    ///
    /// @param other Vector to be compared
    /// @return True if equal
    ///
    [[nodiscard]] auto Equal(const Vector2& other) const noexcept -> bool
    {
        return Math::Equal(x, other.x) && Math::Equal(y, other.y);
    }

    ///
    /// @brief Calculate the magnitude of the vector (squared)
    ///
    /// @return Magnitude of the vector (squared)
    ///
    [[nodiscard]] constexpr auto LengthSQ() const noexcept -> float
    {
        return (x * x) + (y * y);
    }

    ///
    /// @brief Calculate the magnitude of the vector
    ///
    /// @return Magnitude of the vector
    ///
    [[nodiscard]] auto Length() const noexcept -> float
    {
        return sqrtf(LengthSQ());
    }

    ///
    /// @brief Calculate distance between two vectors/points (squared)
    ///
    /// @param other The second vector/point used in calculation
    /// @return Distance between two vectors/points (squared)
    ///
    [[nodiscard]] constexpr auto DistanceSQ(const Vector2& other) const noexcept -> float
    {
        float dx = other.x - x;
        float dy = other.y - y;
        return (dx * dx) + (dy * dy);
    }

    ///
    /// @brief Calculate distance between two vectors/points
    ///
    /// @param other The second vector/point used in calculation
    /// @return Distance between two vectors/point
    ///
    [[nodiscard]] auto Distance(const Vector2& other) const noexcept -> float
    {
        return sqrtf(DistanceSQ(other));
    }

    ///
    /// @brief Calculate the dot product of two vectors
    ///
    /// @param other The second vector used in calculation
    /// @return Dot product of two vectors
    ///
    [[nodiscard]] constexpr auto Dot(const Vector2& other) const noexcept -> float
    {
        return x * other.x + y * other.y;
    }

    ///
    /// @brief Calculate determinant of two vectors (2x2 matrix determinant)
    ///
    /// @param other The second vector used in calculation
    /// @return Determinant of two vectors
    ///
    [[nodiscard]] constexpr auto Determinant(const Vector2& other) const noexcept -> float
    {
        return x * other.y - y * other.x;
    }

    ///
    /// @brief Calculate the angle between the vector and the x-axis
    ///
    /// @return Angle between the vector and the x-axis
    ///
    [[nodiscard]] auto Angle() const noexcept -> float
    {
        return atan2f(y, x);
    }

    ///
    /// @brief Calculate the angle between two vectors
    ///
    /// @param other The second vector
    /// @return Angle between two vectors
    ///
    [[nodiscard]] auto Angle(const Vector2& other) const noexcept -> float
    {
        float dot = Dot(other);
        float det = Determinant(other);
        return atan2f(det, dot);
    }

    ///
    /// @brief Normalize the vector
    ///
    /// @return Reference to this
    ///
    auto Normalize() noexcept -> Vector2&
    {
        float n = LengthSQ();
        if (n < Constants::EPSILON) {
            return *this;
        }
        n = 1.F / sqrtf(n);
        x *= n;
        y *= n;
        return *this;
    }

    ///
    /// @brief Normalize the vector
    ///
    /// @param vector Vector to be normalized
    /// @return Normalized vector
    ///
    static auto Normalize(const Vector2& vector) noexcept -> Vector2
    {
        float n = vector.LengthSQ();
        if (n < Constants::EPSILON) {
            return Vector2();
        }
        n = 1.F / sqrtf(n);
        return Vector2(vector.x * n, vector.y * n);
    }

    ///
    /// @brief Calculate linear interpolation between two vectors
    ///
    /// @param other The second vector used in calculation
    /// @param t Interpolation parameter (0 to 1)
    /// @return Linearly interpolated vector
    ///
    [[nodiscard]] constexpr auto Interpolate(const Vector2& other, float t) const noexcept -> Vector2
    {
        float rX = x + t * (other.x - x);
        float rY = y + t * (other.y - y);
        return Vector2(rX, rY);
    }

    static const Vector2 ONE;    //!< Vector with values ( 1,  1)
    static const Vector2 ZERO;   //!< Vector with values ( 0,  0)
    static const Vector2 UP;     //!< Vector with values ( 0,  1)
    static const Vector2 DOWN;   //!< Vector with values ( 0, -1)
    static const Vector2 RIGHT;  //!< Vector with values ( 1,  0)
    static const Vector2 LEFT;   //!< Vector with values (-1,  0)
    static const Vector2 UNIT_X; //!< Vector with values ( 1,  0)
    static const Vector2 UNIT_Y; //!< Vector with values ( 0,  1)
};

using Vec2 = Vector2; //!< Shorter form of Vector2

//
//
// Vector2 constants
//
//

constexpr inline const Vector2 Vector2::ONE    = {1.F, 1.F};
constexpr inline const Vector2 Vector2::ZERO   = {0.F, 0.F};
constexpr inline const Vector2 Vector2::UP     = {0.F, 1.F};
constexpr inline const Vector2 Vector2::DOWN   = {0.F, -1.F};
constexpr inline const Vector2 Vector2::RIGHT  = {1.F, 0.F};
constexpr inline const Vector2 Vector2::LEFT   = {-1.F, 0.F};
constexpr inline const Vector2 Vector2::UNIT_X = {1.F, 0.F};
constexpr inline const Vector2 Vector2::UNIT_Y = {0.F, 1.F};

} // namespace Spades
