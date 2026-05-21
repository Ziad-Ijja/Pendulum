#include "pendulum/physics/euler_integrator.hpp"

namespace pendulum::physics {

void EulerIntegrator::step(DynamicalSystem& system, double time, double dt)
{
    const StateVector current = system.state();
    system.setState(current + system.derivative(current, time) * dt);
}

} // namespace pendulum::physics

