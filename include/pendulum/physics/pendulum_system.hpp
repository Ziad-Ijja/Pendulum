#pragma once

#include "pendulum/physics/dynamical_system.hpp"
#include "pendulum/physics/energy_sample.hpp"
#include "pendulum/physics/pendulum_link.hpp"
#include "pendulum/physics/pendulum_snapshot.hpp"

#include <cstddef>
#include <optional>

namespace pendulum::physics {

class PendulumSystem : public DynamicalSystem {
public:
    ~PendulumSystem() override = default;

    [[nodiscard]] virtual PendulumSnapshot snapshot(double time) const = 0;
    [[nodiscard]] virtual EnergySample energy(double time) const = 0;
    [[nodiscard]] virtual std::size_t linkCount() const;
    [[nodiscard]] virtual std::optional<std::size_t> nearestMass(core::Vec2 worldPoint, double maxDistance) const;
    virtual bool setMassPositionWithoutVelocity(std::size_t massIndex, core::Vec2 worldPoint) = 0;
    virtual bool addLinkAfterLast(PendulumLink link);
    virtual bool removeLastLink();
};

} // namespace pendulum::physics
