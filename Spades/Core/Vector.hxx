/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Types.hxx"

namespace Spades {

struct Vector2f
{
    float x{0.f};
    float y{0.f};

    constexpr Vector2f() noexcept = default;

    constexpr Vector2f(float t) noexcept : x(t), y(t)
    {
    }

    constexpr Vector2f(float a, float b) noexcept : x(a), y(b)
    {
    }

    constexpr Vector2f(const Vector2f& v) noexcept : x(v.x), y(v.y)
    {
    }

    constexpr Vector2f& operator=(const Vector2f& other) noexcept
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    constexpr Vector2f operator+(const Vector2f& v) const noexcept
    {
        return Vector2f(x + v.x, y + v.y);
    }

    constexpr Vector2f operator-(const Vector2f& v) const noexcept
    {
        return Vector2f(x - v.x, y - v.y);
    }
};

struct Vector3f
{
    float x{0.f};
    float y{0.f};
    float z{0.f};

    constexpr Vector3f() = default;

    constexpr Vector3f(float t) noexcept : x(t), y(t), z(t)
    {
    }

    constexpr Vector3f(float a, float b, float c) noexcept : x(a), y(b), z(c)
    {
    }

    constexpr Vector3f(const Vector2f& ab, float c) noexcept : x(ab.x), y(ab.y), z(c)
    {
    }

    constexpr Vector3f(const Vector3f& v) noexcept : x(v.x), y(v.y), z(v.z)
    {
    }

    constexpr Vector3f& operator=(const Vector3f& other) noexcept
    {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }
};

struct Quad2f
{
    Vector2f a;
    Vector2f b;

    constexpr Quad2f() noexcept = default;

    constexpr Quad2f(const Vector2f& pos, float width, float height) : a{pos}, b{width, height}
    {
    }

    constexpr Quad2f(const Quad2f& other) noexcept : a{other.a}, b{other.b}
    {
    }

    constexpr Quad2f& operator=(const Quad2f& other) noexcept
    {
        a = other.a;
        b = other.b;
        return *this;
    }
};

} // namespace Spades
