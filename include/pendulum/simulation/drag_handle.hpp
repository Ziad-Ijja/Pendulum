#pragma once

#include <cstddef>

namespace pendulum::simulation {

struct DragHandle {
    std::size_t worldIndex = 0;
    std::size_t systemIndex = 0;
    std::size_t massIndex = 0;
};

} // namespace pendulum::simulation

