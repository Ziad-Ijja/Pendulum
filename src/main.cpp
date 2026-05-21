#include "pendulum/core/visualization_settings.hpp"
#include "pendulum/input/input_controller.hpp"
#include "pendulum/physics/chain_pendulum_system.hpp"
#include "pendulum/physics/rk4_integrator.hpp"
#include "pendulum/render/renderer.hpp"
#include "pendulum/simulation/simulation_manager.hpp"
#include "pendulum/ui/debug_overlay.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <cmath>
#include <memory>
#include <vector>

int main()
{
    namespace physics = pendulum::physics;
    namespace simulation = pendulum::simulation;

    sf::RenderWindow window{
        sf::VideoMode{{1920U, 1080U}},
        "Pendulum Chaos Lab"
    };
    window.setVerticalSyncEnabled(true);

    simulation::SimulationManager simulations;
    auto& world = simulations.createWorld("Primary");
    world.setIntegrator(std::make_unique<physics::RK4Integrator>());

    constexpr double pi = 3.14159265358979323846;
    world.addSystem(std::make_unique<physics::ChainPendulumSystem>(
        std::vector<physics::PendulumLink>{{1.8, 1.0}},
        physics::PhysicsParameters{9.81, 0.002},
        std::vector<double>{pi / 3.0},
        std::vector<double>{0.0}
    ));

    pendulum::input::InputController input;
    pendulum::render::Renderer renderer;
    pendulum::ui::DebugOverlay debugOverlay;
    pendulum::core::VisualizationSettings visualization;

    sf::Clock frameClock;
    double accumulator = 0.0;
    constexpr double fixedDt = 1.0 / 240.0;
    constexpr double maxFrameTime = 0.25;

    while (window.isOpen()) {
        input.handleWindowEvents(window, simulations, visualization, renderer.camera());

        const double frameTime = std::min(static_cast<double>(frameClock.restart().asSeconds()), maxFrameTime);
        accumulator += frameTime;

        while (accumulator >= fixedDt) {
            input.applyActiveDrag(window, simulations, renderer.camera());
            simulations.update(fixedDt);
            input.applyActiveDrag(window, simulations, renderer.camera());
            accumulator -= fixedDt;
        }

        renderer.beginFrame(window);
        renderer.renderSimulations(window, simulations, visualization);
        debugOverlay.render(window, visualization, simulations);
        renderer.endFrame(window);
    }

    return 0;
}
