#pragma once

#include "pendulum/core/visualization_settings.hpp"

namespace pendulum::simulation {
class SimulationManager;
}

namespace sf {
class RenderTarget;
}

namespace pendulum::ui {

class DebugOverlay {
public:
    void render(
        sf::RenderTarget& target,
        const core::VisualizationSettings& visualization,
        const simulation::SimulationManager& simulations
    ) const;
};

} // namespace pendulum::ui
