#pragma once

#include "pendulum/core/vec2.hpp"
#include "pendulum/simulation/simulation_world.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace pendulum::simulation {

class SimulationManager {
public:
    SimulationWorld& createWorld(std::string name);
    void addWorld(std::unique_ptr<SimulationWorld> world);
    void update(double dt);
    [[nodiscard]] std::optional<DragHandle> findDraggableMass(core::Vec2 worldPoint, double maxDistance) const;
    bool dragMassWithoutVelocity(const DragHandle& handle, core::Vec2 worldPoint);
    [[nodiscard]] std::size_t primaryLinkCount() const;
    bool addLinkToPrimarySystem(physics::PendulumLink link);
    bool removeLinkFromPrimarySystem();

    [[nodiscard]] std::vector<std::unique_ptr<SimulationWorld>>& worlds() noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<SimulationWorld>>& worlds() const noexcept;

private:
    std::vector<std::unique_ptr<SimulationWorld>> worlds_;
};

} // namespace pendulum::simulation
