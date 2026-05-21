#pragma once

#include "pendulum/core/vec2.hpp"

#include <SFML/System/Vector2.hpp>

namespace pendulum::render {

class Camera2D {
public:
    void setCenter(core::Vec2 center) noexcept;
    void setPixelsPerMeter(double pixelsPerMeter) noexcept;
    void setViewportSize(sf::Vector2u size) noexcept;

    [[nodiscard]] sf::Vector2f worldToScreen(core::Vec2 world) const noexcept;
    [[nodiscard]] core::Vec2 screenToWorld(sf::Vector2f screen) const noexcept;

private:
    core::Vec2 center_{0.0, 0.0};
    double pixelsPerMeter_ = 120.0;
    sf::Vector2u viewportSize_{1280U, 720U};
};

} // namespace pendulum::render
