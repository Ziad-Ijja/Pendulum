#pragma once

#include "pendulum/core/vec2.hpp"

#include <cstddef>
#include <deque>

namespace pendulum::simulation {

class TrajectoryBuffer {
public:
    explicit TrajectoryBuffer(std::size_t capacity = 1800);

    void push(core::Vec2 point);
    void clear();

    [[nodiscard]] const std::deque<core::Vec2>& samples() const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;

private:
    std::deque<core::Vec2> samples_;
    std::size_t capacity_ = 1800;
};

} // namespace pendulum::simulation

