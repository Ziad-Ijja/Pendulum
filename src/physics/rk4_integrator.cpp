#include "pendulum/physics/rk4_integrator.hpp"

namespace pendulum::physics {

void RK4Integrator::step(DynamicalSystem& system, double time, double dt)
{
    const StateVector y = system.state();

    const StateVector k1 = system.derivative(y, time);
    const StateVector k2 = system.derivative(y + k1 * (dt * 0.5), time + dt * 0.5);
    const StateVector k3 = system.derivative(y + k2 * (dt * 0.5), time + dt * 0.5);
    const StateVector k4 = system.derivative(y + k3 * dt, time + dt);

    system.setState(y + (k1 + k2 * 2.0 + k3 * 2.0 + k4) * (dt / 6.0));
}

} // namespace pendulum::physics

