#pragma once

#include "pendulum/core/vec2.hpp"

#include <vector>

namespace pendulum::physics {

struct PendulumSnapshot {
    std::vector<core::Vec2> anchors;
    std::vector<core::Vec2> joints;
    std::vector<core::Vec2> masses;
    double time = 0.0;
};

} // namespace pendulum::physics

