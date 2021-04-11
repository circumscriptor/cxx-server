/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Vector2.hxx"

namespace Spades {

///
/// @brief 3D Vector
///
///
struct Vector3
{
    float x{0.F}; //!< X coordinate
    float y{0.F}; //!< Y coordinate
    float z{0.F}; //!< Z coordinate

    ///
    /// @brief Construct a new Vector3 object
    ///
    ///
    constexpr Vector3() noexcept = default;

    ///
    /// @brief Construct a new Vector3 object using the same value for each coordinate
    ///
    /// @param s Scalar value (X, Y and Z)
    ///
    constexpr explicit Vector3(float s) noexcept : x(s), y(s), z(s)
    {
    }

    ///
    /// @brief Construct a new Vector3 object using Vector2 object and value
    ///
    /// @param a Vector2 object (X and Y)
    /// @param b Scalar value (Z)
    ///
    constexpr Vector3(const Vector2& a, float b) noexcept : x(a.x), y(a.y), z(b)
    {
    }

    ///
    /// @brief Construct a new Vector3 object using Vector2 object and value
    ///
    /// @param a Scalar value (X)
    /// @param b Vector2 object (Y and Z)
    ///
    constexpr Vector3(float a, const Vector2& b) noexcept : x(a), y(b.x), z(b.y)
    {
    }

    ///
    /// @brief Construct a new Vector3 object using three values (one for each coordinate)
    ///
    /// @param a Scalar value (X)
    /// @param b Scalar value (Y)
    /// @param c Scalar value (Z)
    ///
    constexpr Vector3(float a, float b, float c) noexcept : x(a), y(b), z(c)
    {
    }

    ///
    /// @brief Construct a new Vector3 object with the contents of another Vector3 object
    ///
    /// @param other Vector3 object to be copied
    ///
    constexpr Vector3(const Vector3& other) = default;

    ///
    /// @brief Construct a new Vector3 object using move semantics
    ///
    /// @param other Vector3 object to be moved
    ///
    constexpr Vector3(Vector3&& other) = default;

    ///
    /// @brief Assign the contents of another Vector3 object
    ///
    /// @param other Vector3 object to be copied
    /// @return Reference to this
    ///
    constexpr auto operator=(const Vector3& other) noexcept -> Vector3& = default;

    ///
    /// @brief Assign the contents of another Vector3 object using move semantics
    ///
    /// @param other Vector3 object to be moved
    /// @return Reference to this
    ///
    constexpr auto operator=(Vector3&& other) noexcept -> Vector3& = default;

    ///
    /// @brief Vector addition
    ///
    /// @param other Vector to be added
    /// @return Reference to this
    ///
    constexpr auto operator+=(const Vector3& other) noexcept -> Vector3&
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    ///
    /// @brief Vector subtraction
    ///
    /// @param other Vector to be subtracted
    /// @return Reference to this
    ///
    constexpr auto operator-=(const Vector3& other) noexcept -> Vector3&
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    ///
    /// @brief Multiply each coordinate by scalar
    ///
    /// @param s Scalar used in calculation
    /// @return Reference to this
    ///
    constexpr auto operator*=(float s) noexcept -> Vector3&
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    ///
    /// @brief Divide each coordinate by scalar
    ///
    /// @param s Scalar used in calculation
    /// @return Reference to this
    ///
    constexpr auto operator/=(float s) noexcept -> Vector3&
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    ///
    /// @brief Vector addition
    ///
    /// @param other Vector to be added
    /// @return Result of addition
    ///
    constexpr auto operator+(const Vector3& other) const noexcept -> Vector3
    {
        return {x + other.x, y + other.y, z + other.z};
    }

    ///
    /// @brief Vector subtraction
    ///
    /// @param other Vector to be subtracted
    /// @return Result of subtraction
    ///
    constexpr auto operator-(const Vector3& other) const noexcept -> Vector3
    {
        return {x - other.x, y - other.y, z - other.z};
    }

    ///
    /// @brief Multiply each coordinate by scalar
    ///
    /// @param s Scalar used in calculation
    /// @return Result of multiplication
    ///
    constexpr auto operator*(float s) const noexcept -> Vector3
    {
        return {x * s, y * s, z * s};
    }

    ///
    /// @brief Divide each coordinate by scalar
    ///
    /// @param s Scalar used in calculation
    /// @return Result of division
    ///
    constexpr auto operator/(float s) const noexcept -> Vector3
    {
        return {x / s, y / s, z / s};
    }

    ///
    /// @brief Get opposite vector
    ///
    /// @return Opposite vector
    ///
    constexpr auto operator-() const noexcept -> Vector3
    {
        return {-x, -y, -z};
    }

    ///
    /// @brief Access coordinate by the index
    ///
    /// @param index The index of the coordinate
    /// @return Reference to the required coordinate
    ///
    auto operator[](unsigned index) noexcept -> float&
    {
        assert(index < 3);
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
        assert(index < 3);
        return reinterpret_cast<const float*>(this)[index];
    }

    ///
    /// @brief Check whether two vectors are equal
    ///
    /// @param other Vector to be compared
    /// @return True if equal
    ///
    constexpr auto operator==(const Vector3& other) const noexcept -> bool
    {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }

    ///
    /// @brief Check whether two vectors are not equal
    ///
    /// @param other Vector to be compared
    /// @return False if equal
    ///
    constexpr auto operator!=(const Vector3& other) const noexcept -> bool
    {
        return (x != other.x) || (y != other.y) || (z != other.z);
    }

    ///
    /// @brief Check whether two vectors are equal (using epsilon)
    ///
    /// @param other Vector to be compared
    /// @return True if equal
    ///
    [[nodiscard]] auto Equal(const Vector3& other) const noexcept -> bool
    {
        return Math::Equal(x, other.x) && Math::Equal(y, other.y) && Math::Equal(z, other.z);
    }

    ///
    /// @brief Calculate the magnitude of the vector (squared)
    ///
    /// @return Magnitude of the vector (squared)
    ///
    [[nodiscard]] constexpr auto LengthSQ() const noexcept -> float
    {
        return (x * x) + (y * y) + (z * z);
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
    [[nodiscard]] constexpr auto DistanceSQ(const Vector3& other) const noexcept -> float
    {
        float dx = other.x - x;
        float dy = other.y - y;
        float dz = other.z - z;
        return (dx * dx) + (dy * dy) + (dz * dz);
    }

    ///
    /// @brief Calculate distance between two vectors/points
    ///
    /// @param other The second vector/point used in calculation
    /// @return Distance between two vectors/point
    ///
    [[nodiscard]] auto Distance(const Vector3& other) const noexcept -> float
    {
        return sqrtf(DistanceSQ(other));
    }

    ///
    /// @brief Calculate the dot product of two vectors
    ///
    /// @param other The second vector used in calculation
    /// @return Dot product of two vectors
    ///
    [[nodiscard]] constexpr auto Dot(const Vector3& other) const noexcept -> float
    {
        return (x * other.x) + (y * other.y) + (z * other.z);
    }

    ///
    /// @brief Calculate the cross product of two vectors
    ///
    /// @param other The second vector used in calculation
    /// @return Cross product of two vectors
    ///
    [[nodiscard]] constexpr auto Cross(const Vector3& other) const noexcept -> Vector3
    {
        float cX = (y * other.z) - (z * other.y);
        float cY = (z * other.x) - (x * other.z);
        float cZ = (x * other.y) - (y * other.x);
        return {cX, cY, cZ};
    }

    ///
    /// @brief Calculate determinant of two vectors
    ///
    /// @param other The second vector used in calculation
    /// @return Determinant of two vectors
    ///
    [[nodiscard]] constexpr auto Determinant(const Vector3& other) const noexcept -> float
    {
        float cX = (y * other.z) - (z * other.y);
        float cY = (z * other.x) - (x * other.z);
        float cZ = (x * other.y) - (y * other.x);
        return cX - cY + cZ;
    }

    ///
    /// @brief Calculate the angle between two vectors
    ///
    /// @param other The second vector
    /// @return Angle between two vectors
    ///
    [[nodiscard]] auto Angle(const Vector3& other) const noexcept -> float
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
    auto Normalize() noexcept -> Vector3&
    {
        float n = LengthSQ();
        if (n < Constants::EPSILON) {
            return *this;
        }
        n = 1.F / sqrtf(n);
        x *= n;
        y *= n;
        z *= n;
        return *this;
    }

    ///
    /// @brief Normalize the vector
    ///
    /// @param vector Vector to be normalized
    /// @return Normalized vector
    ///
    static auto Normalize(const Vector3& vector) noexcept -> Vector3
    {
        float n = vector.LengthSQ();
        if (n < Constants::EPSILON) {
            return {};
        }
        n = 1.F / sqrtf(n);
        return {vector.x * n, vector.y * n, vector.z * n};
    }

    ///
    /// @brief Calculate linear interpolation between two vectors
    ///
    /// @param other The second vector used in calculation
    /// @param t Interpolation parameter (0 to 1)
    /// @return Linearly interpolated vector
    ///
    [[nodiscard]] constexpr auto Interpolate(const Vector3& other, float t) const noexcept -> Vector3
    {
        float rX = x + t * (other.x - x);
        float rY = y + t * (other.y - y);
        float rZ = z + t * (other.z - z);
        return {rX, rY, rZ};
    }

    static const Vector3 ONE;     //!< Vector with values (  1,  1,  1)
    static const Vector3 ZERO;    //!< Vector with values (  0,  0,  0)
    static const Vector3 UP;      //!< Vector with values (  0,  1,  0)
    static const Vector3 DOWN;    //!< Vector with values (  0, -1,  0)
    static const Vector3 RIGHT;   //!< Vector with values (  1,  0,  0)
    static const Vector3 LEFT;    //!< Vector with values ( -1,  0,  0)
    static const Vector3 BACK;    //!< Vector with values (  0,  0,  1)
    static const Vector3 FORWARD; //!< Vector with values (  0,  0, -1)
    static const Vector3 UNIT_X;  //!< Vector with values (  1,  0,  0)
    static const Vector3 UNIT_Y;  //!< Vector with values (  0,  1,  0)
    static const Vector3 UNIT_Z;  //!< Vector with values (  0,  0,  1)
};

using Vec3 = Vector3; //!< Shorter form of Vector3

//
//
// Vector3 constants
//
//

constexpr inline const Vector3 Vector3::ONE     = {1.F, 1.F, 1.F};
constexpr inline const Vector3 Vector3::ZERO    = {0.F, 0.F, 0.F};
constexpr inline const Vector3 Vector3::UP      = {0.F, 1.F, 0.F};
constexpr inline const Vector3 Vector3::DOWN    = {0.F, -1.F, 0.F};
constexpr inline const Vector3 Vector3::RIGHT   = {1.F, 0.F, 0.F};
constexpr inline const Vector3 Vector3::LEFT    = {-1.F, 0.F, 0.F};
constexpr inline const Vector3 Vector3::BACK    = {0.F, 0.F, 1.F};
constexpr inline const Vector3 Vector3::FORWARD = {0.F, 0.F, -1.F};
constexpr inline const Vector3 Vector3::UNIT_X  = {1.F, 0.F, 0.F};
constexpr inline const Vector3 Vector3::UNIT_Y  = {0.F, 1.F, 0.F};
constexpr inline const Vector3 Vector3::UNIT_Z  = {0.F, 0.F, 1.F};

} // namespace Spades