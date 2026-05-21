#include "pendulum/core/vec2.hpp"

#include <cmath>

namespace pendulum::core {

Vec2 operator+(Vec2 lhs, Vec2 rhs) noexcept
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

Vec2 operator-(Vec2 lhs, Vec2 rhs) noexcept
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

Vec2 operator*(Vec2 value, double scalar) noexcept
{
    return {value.x * scalar, value.y * scalar};
}

Vec2 operator/(Vec2 value, double scalar) noexcept
{
    return {value.x / scalar, value.y / scalar};
}

double length(Vec2 value) noexcept
{
    return std::hypot(value.x, value.y);
}

} // namespace pendulum::core

