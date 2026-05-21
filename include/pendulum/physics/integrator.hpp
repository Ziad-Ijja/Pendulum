#pragma once

#include "pendulum/physics/dynamical_system.hpp"

namespace pendulum::physics {

class Integrator {
public:
    virtual ~Integrator() = default;

    virtual void step(DynamicalSystem& system, double time, double dt) = 0;
};

} // namespace pendulum::physics

