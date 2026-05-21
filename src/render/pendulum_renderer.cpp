#include "pendulum/render/pendulum_renderer.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Angle.hpp>

#include <cmath>
#include <cstdint>

namespace pendulum::render {

namespace {

[[nodiscard]] float distance(sf::Vector2f lhs, sf::Vector2f rhs)
{
    const float dx = rhs.x - lhs.x;
    const float dy = rhs.y - lhs.y;
    return std::hypot(dx, dy);
}

[[nodiscard]] float angleRadians(sf::Vector2f lhs, sf::Vector2f rhs)
{
    return std::atan2(rhs.y - lhs.y, rhs.x - lhs.x);
}

void drawSegment(sf::RenderTarget& target, sf::Vector2f start, sf::Vector2f end, float thickness, sf::Color color)
{
    const float segmentLength = distance(start, end);
    if (segmentLength <= 0.0F) {
        return;
    }

    sf::RectangleShape segment{{segmentLength, thickness}};
    segment.setOrigin({0.0F, thickness * 0.5F});
    segment.setPosition(start);
    segment.setRotation(sf::radians(angleRadians(start, end)));
    segment.setFillColor(color);
    target.draw(segment);
}

} // namespace

void PendulumRenderer::renderTrail(
    sf::RenderTarget& target,
    const simulation::TrajectoryBuffer& trail,
    const Camera2D& camera
) const
{
    const auto& samples = trail.samples();
    if (samples.size() < 2) {
        return;
    }

    sf::VertexArray trailVertices{sf::PrimitiveType::LineStrip, samples.size()};
    for (std::size_t i = 0; i < samples.size(); ++i) {
        const float age = static_cast<float>(i) / static_cast<float>(samples.size() - 1);
        const auto alpha = static_cast<std::uint8_t>(20.0F + age * 155.0F);
        sf::Color color = trailColor_;
        color.a = alpha;

        trailVertices[i].position = camera.worldToScreen(samples[i]);
        trailVertices[i].color = color;
    }
    target.draw(trailVertices);
}

void PendulumRenderer::render(sf::RenderTarget& target, const physics::PendulumSnapshot& snapshot, const Camera2D& camera)
    const
{
    const auto drawDisc = [&target](sf::Vector2f position, float radius, sf::Color color) {
        sf::CircleShape shape{radius};
        shape.setOrigin({radius, radius});
        shape.setPosition(position);
        shape.setFillColor(color);
        target.draw(shape);
    };

    const auto drawRod = [this, &target](sf::Vector2f start, sf::Vector2f end) {
        drawSegment(target, start, end, style_.rodThickness, style_.rodColor);
    };

    const std::size_t count = snapshot.masses.size();
    for (std::size_t i = 0; i < count; ++i) {
        const core::Vec2 anchor = i == 0 ? snapshot.anchors.front() : snapshot.masses[i - 1];
        const sf::Vector2f start = camera.worldToScreen(anchor);
        const sf::Vector2f end = camera.worldToScreen(snapshot.masses[i]);
        drawRod(start, end);
    }

    for (const core::Vec2& anchor : snapshot.anchors) {
        drawDisc(camera.worldToScreen(anchor), style_.anchorRadius, style_.anchorColor);
    }

    for (const core::Vec2& mass : snapshot.masses) {
        drawDisc(camera.worldToScreen(mass), style_.massRadius, style_.massColor);
    }
}

} // namespace pendulum::render
