#include "pendulum/render/camera2d.hpp"

namespace pendulum::render {

void Camera2D::setCenter(core::Vec2 center) noexcept
{
    center_ = center;
}

void Camera2D::setPixelsPerMeter(double pixelsPerMeter) noexcept
{
    pixelsPerMeter_ = pixelsPerMeter;
}

void Camera2D::setViewportSize(sf::Vector2u size) noexcept
{
    viewportSize_ = size;
}

sf::Vector2f Camera2D::worldToScreen(core::Vec2 world) const noexcept
{
    const auto halfWidth = static_cast<float>(viewportSize_.x) * 0.5F;
    const auto halfHeight = static_cast<float>(viewportSize_.y) * 0.35F;

    return {
        halfWidth + static_cast<float>((world.x - center_.x) * pixelsPerMeter_),
        halfHeight + static_cast<float>((world.y - center_.y) * pixelsPerMeter_)
    };
}

core::Vec2 Camera2D::screenToWorld(sf::Vector2f screen) const noexcept
{
    const auto halfWidth = static_cast<float>(viewportSize_.x) * 0.5F;
    const auto halfHeight = static_cast<float>(viewportSize_.y) * 0.35F;

    return {
        center_.x + (static_cast<double>(screen.x - halfWidth) / pixelsPerMeter_),
        center_.y + (static_cast<double>(screen.y - halfHeight) / pixelsPerMeter_)
    };
}

} // namespace pendulum::render
