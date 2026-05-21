#pragma once

#include "pendulum/physics/pendulum_snapshot.hpp"
#include "pendulum/render/camera2d.hpp"
#include "pendulum/simulation/trajectory_buffer.hpp"

#include <SFML/Graphics/Color.hpp>

namespace sf {
class RenderTarget;
}

namespace pendulum::render {

struct PendulumRenderStyle {
    sf::Color rodColor;
    sf::Color massColor;
    sf::Color anchorColor;
    float rodThickness = 3.0F;
    float massRadius = 12.0F;
    float anchorRadius = 5.0F;
};

class PendulumRenderer {
public:
    void renderTrail(sf::RenderTarget& target, const simulation::TrajectoryBuffer& trail, const Camera2D& camera) const;
    void render(sf::RenderTarget& target, const physics::PendulumSnapshot& snapshot, const Camera2D& camera) const;

private:
    PendulumRenderStyle style_{
        sf::Color{45, 50, 58},
        sf::Color{206, 70, 55},
        sf::Color{35, 137, 126},
        3.0F,
        12.0F,
        5.0F
    };
    sf::Color trailColor_{45, 96, 160};
    float trailThickness_ = 2.0F;
};

} // namespace pendulum::render
