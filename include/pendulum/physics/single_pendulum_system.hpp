#pragma once

#include "pendulum/physics/pendulum_link.hpp"
#include "pendulum/physics/pendulum_system.hpp"
#include "pendulum/physics/physics_parameters.hpp"

namespace pendulum::physics {

class SinglePendulumSystem final : public PendulumSystem {
public:
    SinglePendulumSystem(PendulumLink link, PhysicsParameters parameters, double angleRadians, double angularVelocity);

    [[nodiscard]] StateVector state() const override;
    void setState(const StateVector& state) override;
    [[nodiscard]] StateVector derivative(const StateVector& state, double time) const override;
    [[nodiscard]] PendulumSnapshot snapshot(double time) const override;
    [[nodiscard]] EnergySample energy(double time) const override;
    [[nodiscard]] std::size_t linkCount() const override;
    bool setMassPositionWithoutVelocity(std::size_t massIndex, core::Vec2 worldPoint) override;

    [[nodiscard]] const PendulumLink& link() const noexcept;
    [[nodiscard]] const PhysicsParameters& parameters() const noexcept;

private:
    PendulumLink link_;
    PhysicsParameters parameters_;
    double angleRadians_ = 0.0;
    double angularVelocity_ = 0.0;
};

} // namespace pendulum::physics
