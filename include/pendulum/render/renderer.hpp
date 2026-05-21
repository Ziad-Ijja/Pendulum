#pragma once

#include "pendulum/core/visualization_settings.hpp"
#include "pendulum/render/camera2d.hpp"
#include "pendulum/render/pendulum_renderer.hpp"
#include "pendulum/simulation/simulation_manager.hpp"

#include <SFML/Graphics/Color.hpp>

namespace sf {
class RenderWindow;
}

namespace pendulum::render {

class Renderer {
public:
    void beginFrame(sf::RenderWindow& window);
    void renderSimulations(
        sf::RenderWindow& window,
        const simulation::SimulationManager& simulations,
        const core::VisualizationSettings& visualization
    );
    void endFrame(sf::RenderWindow& window);

    [[nodiscard]] const Camera2D& camera() const noexcept;

private:
    Camera2D camera_;
    PendulumRenderer pendulumRenderer_;
    sf::Color background_{250, 250, 248};
};

} // namespace pendulum::render
