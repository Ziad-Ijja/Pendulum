#pragma once

namespace pendulum::core {

struct Vec2 {
    double x = 0.0;
    double y = 0.0;
};

[[nodiscard]] Vec2 operator+(Vec2 lhs, Vec2 rhs) noexcept;
[[nodiscard]] Vec2 operator-(Vec2 lhs, Vec2 rhs) noexcept;
[[nodiscard]] Vec2 operator*(Vec2 value, double scalar) noexcept;
[[nodiscard]] Vec2 operator/(Vec2 value, double scalar) noexcept;

[[nodiscard]] double length(Vec2 value) noexcept;

} // namespace pendulum::core

