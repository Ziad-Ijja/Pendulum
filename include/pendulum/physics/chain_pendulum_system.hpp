#pragma once

#include "pendulum/physics/pendulum_link.hpp"
#include "pendulum/physics/pendulum_system.hpp"
#include "pendulum/physics/physics_parameters.hpp"

#include <vector>

namespace pendulum::physics {

class ChainPendulumSystem final : public PendulumSystem {
public:
    ChainPendulumSystem(
        std::vector<PendulumLink> links,
        PhysicsParameters parameters,
        std::vector<double> anglesRadians,
        std::vector<double> angularVelocities
    );

    [[nodiscard]] StateVector state() const override;
    void setState(const StateVector& state) override;
    [[nodiscard]] StateVector derivative(const StateVector& state, double time) const override;
    [[nodiscard]] PendulumSnapshot snapshot(double time) const override;
    [[nodiscard]] std::size_t linkCount() const override;

    bool setMassPositionWithoutVelocity(std::size_t massIndex, core::Vec2 worldPoint) override;
    bool addLinkAfterLast(PendulumLink link) override;
    bool removeLastLink() override;

    [[nodiscard]] const std::vector<PendulumLink>& links() const noexcept;

private:
    [[nodiscard]] std::vector<double> anglesFromState(const StateVector& state) const;
    [[nodiscard]] std::vector<double> angularVelocitiesFromState(const StateVector& state) const;

    std::vector<PendulumLink> links_;
    PhysicsParameters parameters_;
    StateVector state_;
};

} // namespace pendulum::physics

