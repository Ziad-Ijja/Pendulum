#pragma once

#include "pendulum/physics/integrator.hpp"

namespace pendulum::physics {

class RK4Integrator final : public Integrator {
public:
    void step(DynamicalSystem& system, double time, double dt) override;
};

} // namespace pendulum::physics

