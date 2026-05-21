#include "pendulum/physics/single_pendulum_system.hpp"

#include <cmath>
#include <stdexcept>

namespace pendulum::physics {

SinglePendulumSystem::SinglePendulumSystem(
    PendulumLink link,
    PhysicsParameters parameters,
    double angleRadians,
    double angularVelocity
)
    : link_(link)
    , parameters_(parameters)
    , angleRadians_(angleRadians)
    , angularVelocity_(angularVelocity)
{
    if (link_.length <= 0.0) {
        throw std::invalid_argument{"Pendulum length must be positive"};
    }
    if (link_.mass <= 0.0) {
        throw std::invalid_argument{"Pendulum mass must be positive"};
    }
}

StateVector SinglePendulumSystem::state() const
{
    return {angleRadians_, angularVelocity_};
}

void SinglePendulumSystem::setState(const StateVector& state)
{
    if (state.size() != 2) {
        throw std::invalid_argument{"SinglePendulumSystem state must contain [angle, angular_velocity]"};
    }

    angleRadians_ = state[0];
    angularVelocity_ = state[1];
}

StateVector SinglePendulumSystem::derivative(const StateVector& state, double /*time*/) const
{
    if (state.size() != 2) {
        throw std::invalid_argument{"SinglePendulumSystem derivative expects a two-value state"};
    }

    const double angle = state[0];
    const double angularVelocity = state[1];
    const double angularAcceleration =
        -(parameters_.gravity / link_.length) * std::sin(angle)
        - parameters_.damping * angularVelocity;

    return {angularVelocity, angularAcceleration};
}

PendulumSnapshot SinglePendulumSystem::snapshot(double time) const
{
    const core::Vec2 anchor{0.0, 0.0};
    const core::Vec2 mass{
        link_.length * std::sin(angleRadians_),
        link_.length * std::cos(angleRadians_)
    };

    PendulumSnapshot result;
    result.anchors.push_back(anchor);
    result.joints.push_back(anchor);
    result.masses.push_back(mass);
    result.time = time;
    return result;
}

EnergySample SinglePendulumSystem::energy(double time) const
{
    const double speed = link_.length * angularVelocity_;
    const double kinetic = 0.5 * link_.mass * speed * speed;
    const double potential = -link_.mass * parameters_.gravity * link_.length * std::cos(angleRadians_);
    return {kinetic, potential, kinetic + potential, 0.0, time};
}

std::size_t SinglePendulumSystem::linkCount() const
{
    return 1;
}

bool SinglePendulumSystem::setMassPositionWithoutVelocity(std::size_t massIndex, core::Vec2 worldPoint)
{
    if (massIndex != 0) {
        return false;
    }

    constexpr double minimumDistance = 1.0e-6;
    if (core::length(worldPoint) < minimumDistance) {
        return false;
    }

    angleRadians_ = std::atan2(worldPoint.x, worldPoint.y);
    angularVelocity_ = 0.0;
    return true;
}

const PendulumLink& SinglePendulumSystem::link() const noexcept
{
    return link_;
}

const PhysicsParameters& SinglePendulumSystem::parameters() const noexcept
{
    return parameters_;
}

} // namespace pendulum::physics
