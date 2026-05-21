#pragma once

#include "pendulum/core/visualization_settings.hpp"
#include "pendulum/render/camera2d.hpp"
#include "pendulum/simulation/drag_handle.hpp"

#include <optional>

namespace pendulum::simulation {
class SimulationManager;
}

namespace sf {
class RenderWindow;
}

namespace pendulum::input {

class InputController {
public:
    void handleWindowEvents(
        sf::RenderWindow& window,
        simulation::SimulationManager& simulations,
        core::VisualizationSettings& visualization,
        const render::Camera2D& camera
    );
    void applyActiveDrag(
        const sf::RenderWindow& window,
        simulation::SimulationManager& simulations,
        const render::Camera2D& camera
    );

private:
    std::optional<simulation::DragHandle> activeDrag_;
};

} // namespace pendulum::input
