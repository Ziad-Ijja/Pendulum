#include "pendulum/simulation/simulation_manager.hpp"

#include <stdexcept>
#include <utility>

namespace pendulum::simulation {

SimulationWorld& SimulationManager::createWorld(std::string name)
{
    auto world = std::make_unique<SimulationWorld>(std::move(name));
    SimulationWorld& reference = *world;
    worlds_.push_back(std::move(world));
    return reference;
}

void SimulationManager::addWorld(std::unique_ptr<SimulationWorld> world)
{
    if (!world) {
        throw std::invalid_argument{"SimulationManager world cannot be null"};
    }

    worlds_.push_back(std::move(world));
}

void SimulationManager::update(double dt)
{
    for (auto& world : worlds_) {
        world->update(dt);
    }
}

std::optional<DragHandle> SimulationManager::findDraggableMass(core::Vec2 worldPoint, double maxDistance) const
{
    for (std::size_t worldIndex = 0; worldIndex < worlds_.size(); ++worldIndex) {
        const auto handle = worlds_[worldIndex]->findDraggableMass(worldIndex, worldPoint, maxDistance);
        if (handle.has_value()) {
            return handle;
        }
    }

    return std::nullopt;
}

bool SimulationManager::dragMassWithoutVelocity(const DragHandle& handle, core::Vec2 worldPoint)
{
    if (handle.worldIndex >= worlds_.size()) {
        return false;
    }

    return worlds_[handle.worldIndex]->dragMassWithoutVelocity(handle, worldPoint);
}

std::size_t SimulationManager::primaryLinkCount() const
{
    if (worlds_.empty()) {
        return 0;
    }

    return worlds_.front()->primaryLinkCount();
}

bool SimulationManager::addLinkToPrimarySystem(physics::PendulumLink link)
{
    if (worlds_.empty()) {
        return false;
    }

    return worlds_.front()->addLinkToPrimarySystem(link);
}

bool SimulationManager::removeLinkFromPrimarySystem()
{
    if (worlds_.empty()) {
        return false;
    }

    return worlds_.front()->removeLinkFromPrimarySystem();
}

std::vector<std::unique_ptr<SimulationWorld>>& SimulationManager::worlds() noexcept
{
    return worlds_;
}

const std::vector<std::unique_ptr<SimulationWorld>>& SimulationManager::worlds() const noexcept
{
    return worlds_;
}

} // namespace pendulum::simulation
