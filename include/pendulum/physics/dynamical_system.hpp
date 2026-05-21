#pragma once

#include "pendulum/physics/state_vector.hpp"

namespace pendulum::physics {

class DynamicalSystem {
public:
    virtual ~DynamicalSystem() = default;

    [[nodiscard]] virtual StateVector state() const = 0;
    virtual void setState(const StateVector& state) = 0;

    [[nodiscard]] virtual StateVector derivative(const StateVector& state, double time) const = 0;
};

} // namespace pendulum::physics

