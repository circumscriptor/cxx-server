/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Types.hxx"

namespace Spades {

template<typename T>
struct TVector2
{
    T x{T(0)};
    T y{T(0)};

    constexpr TVector2() noexcept = default;

    constexpr TVector2(T t) noexcept : x(t), y(t)
    {
    }

    constexpr TVector2(T a, T b) noexcept : x(a), y(b)
    {
    }

    constexpr TVector2(const TVector2& v) noexcept : x(v.x), y(v.y)
    {
    }

    constexpr TVector2& operator=(const TVector2& other) noexcept
    {
        x = other.x;
        y = other.y;
        return *this;
    }
};

template<typename T>
struct TVector3
{
    T x{T(0)};
    T y{T(0)};
    T z{T(0)};

    constexpr TVector3() = default;

    constexpr TVector3(T t) noexcept : x(t), y(t), z(t)
    {
    }

    constexpr TVector3(T a, T b, T c) noexcept : x(a), y(b), z(c)
    {
    }

    constexpr TVector3(const TVector2<T>& ab, T c) noexcept : x(ab.x), y(ab.y), z(c)
    {
    }

    constexpr TVector3(const TVector3& v) noexcept : x(v.x), y(v.y), z(v.z)
    {
    }

    constexpr TVector3& operator=(const TVector3& other) noexcept
    {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }
};

using Vector2f = TVector2<float>;
using Vector2b = TVector2<uint8>;
using Vector3f = TVector3<float>;
using Vector3b = TVector3<uint8>;

} // namespace Spades
