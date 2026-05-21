#include "pendulum/input/input_controller.hpp"

#include "pendulum/simulation/simulation_manager.hpp"
#include "pendulum/ui/control_layout.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace pendulum::input {

namespace {

bool contains(core::ScreenRect bounds, sf::Vector2i position)
{
    return bounds.contains(static_cast<float>(position.x), static_cast<float>(position.y));
}

} // namespace

void InputController::handleWindowEvents(
    sf::RenderWindow& window,
    simulation::SimulationManager& simulations,
    core::VisualizationSettings& visualization,
    const render::Camera2D& camera
)
{
    constexpr double dragSelectionRadius = 0.16;

    while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Space) {
                for (auto& world : simulations.worlds()) {
                    world->setPaused(!world->isPaused());
                }
            }
        }

        if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePressed->button != sf::Mouse::Button::Left) {
                continue;
            }

            if (contains(ui::trailCheckboxBounds, mousePressed->position)) {
                visualization.showTrails = !visualization.showTrails;
                continue;
            }
            if (contains(ui::incrementLinkBounds, mousePressed->position)) {
                constexpr std::size_t maxInteractiveLinks = 8;
                if (simulations.primaryLinkCount() < maxInteractiveLinks) {
                    simulations.addLinkToPrimarySystem({0.9, 0.8});
                }
                activeDrag_.reset();
                continue;
            }
            if (contains(ui::decrementLinkBounds, mousePressed->position)) {
                simulations.removeLinkFromPrimarySystem();
                activeDrag_.reset();
                continue;
            }
            if (contains(ui::linkCounterBounds, mousePressed->position)) {
                continue;
            }

            const auto worldPoint = camera.screenToWorld({
                static_cast<float>(mousePressed->position.x),
                static_cast<float>(mousePressed->position.y)
            });
            activeDrag_ = simulations.findDraggableMass(worldPoint, dragSelectionRadius);
            applyActiveDrag(window, simulations, camera);
        }

        if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseReleased->button == sf::Mouse::Button::Left) {
                activeDrag_.reset();
            }
        }
    }

    applyActiveDrag(window, simulations, camera);
}

void InputController::applyActiveDrag(
    const sf::RenderWindow& window,
    simulation::SimulationManager& simulations,
    const render::Camera2D& camera
)
{
    if (!activeDrag_.has_value()) {
        return;
    }

    if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        activeDrag_.reset();
        return;
    }

    const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    const auto worldPoint = camera.screenToWorld({
        static_cast<float>(mousePosition.x),
        static_cast<float>(mousePosition.y)
    });

    if (!simulations.dragMassWithoutVelocity(*activeDrag_, worldPoint)) {
        activeDrag_.reset();
    }
}

} // namespace pendulum::input
