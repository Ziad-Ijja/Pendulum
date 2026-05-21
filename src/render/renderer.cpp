#include "pendulum/render/renderer.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

namespace pendulum::render {

void Renderer::beginFrame(sf::RenderWindow& window)
{
    camera_.setViewportSize(window.getSize());
    window.clear(background_);
}

void Renderer::renderSimulations(
    sf::RenderWindow& window,
    const simulation::SimulationManager& simulations,
    const core::VisualizationSettings& visualization
)
{
    for (const auto& world : simulations.worlds()) {
        if (visualization.showTrails) {
            for (const auto& trail : world->trails()) {
                pendulumRenderer_.renderTrail(window, trail, camera_);
            }
        }

        const auto snapshots = world->snapshots();
        for (const auto& snapshot : snapshots) {
            pendulumRenderer_.render(window, snapshot, camera_);
        }
    }
}

void Renderer::endFrame(sf::RenderWindow& window)
{
    window.display();
}

const Camera2D& Renderer::camera() const noexcept
{
    return camera_;
}

} // namespace pendulum::render
