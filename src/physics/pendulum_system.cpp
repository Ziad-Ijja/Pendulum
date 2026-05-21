#include "pendulum/physics/pendulum_system.hpp"

#include "pendulum/core/vec2.hpp"

#include <limits>

namespace pendulum::physics {

std::size_t PendulumSystem::linkCount() const
{
    return snapshot(0.0).masses.size();
}

std::optional<std::size_t> PendulumSystem::nearestMass(core::Vec2 worldPoint, double maxDistance) const
{
    const auto current = snapshot(0.0);
    std::optional<std::size_t> nearest;
    double nearestDistance = std::numeric_limits<double>::max();

    for (std::size_t i = 0; i < current.masses.size(); ++i) {
        const double distance = core::length(current.masses[i] - worldPoint);
        if (distance <= maxDistance && distance < nearestDistance) {
            nearest = i;
            nearestDistance = distance;
        }
    }

    return nearest;
}

bool PendulumSystem::addLinkAfterLast(PendulumLink /*link*/)
{
    return false;
}

bool PendulumSystem::removeLastLink()
{
    return false;
}

} // namespace pendulum::physics
