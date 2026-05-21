#include "pendulum/ui/debug_overlay.hpp"

#include "pendulum/simulation/simulation_manager.hpp"
#include "pendulum/ui/control_layout.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Angle.hpp>

#include <algorithm>

namespace pendulum::ui {

namespace {

void drawIconSegment(sf::RenderTarget& target, sf::Vector2f start, sf::Vector2f end, sf::Color color)
{
    sf::RectangleShape segment{{18.0F, 2.0F}};
    segment.setOrigin({0.0F, 1.0F});
    segment.setPosition(start);
    segment.setRotation(sf::degrees(end.y > start.y ? 25.0F : -25.0F));
    segment.setFillColor(color);
    target.draw(segment);
}

void drawLine(sf::RenderTarget& target, sf::Vector2f start, sf::Vector2f size, sf::Color color)
{
    sf::RectangleShape line{size};
    line.setPosition(start);
    line.setFillColor(color);
    target.draw(line);
}

void drawButtonBox(sf::RenderTarget& target, core::ScreenRect bounds, sf::Color border)
{
    sf::RectangleShape box{{bounds.width, bounds.height}};
    box.setPosition({bounds.left, bounds.top});
    box.setFillColor(sf::Color{255, 255, 255, 230});
    box.setOutlineColor(border);
    box.setOutlineThickness(2.0F);
    target.draw(box);
}

void drawDigit(sf::RenderTarget& target, int digit, core::ScreenRect bounds, sf::Color color)
{
    constexpr bool segments[10][7] = {
        {true, true, true, true, true, true, false},
        {false, true, true, false, false, false, false},
        {true, true, false, true, true, false, true},
        {true, true, true, true, false, false, true},
        {false, true, true, false, false, true, true},
        {true, false, true, true, false, true, true},
        {true, false, true, true, true, true, true},
        {true, true, true, false, false, false, false},
        {true, true, true, true, true, true, true},
        {true, true, true, true, false, true, true},
    };

    digit = std::clamp(digit, 0, 9);
    const float x = bounds.left + 6.0F;
    const float y = bounds.top + 5.0F;
    const float w = bounds.width - 12.0F;
    const float h = bounds.height - 10.0F;
    const float t = 2.0F;

    if (segments[digit][0]) {
        drawLine(target, {x, y}, {w, t}, color);
    }
    if (segments[digit][1]) {
        drawLine(target, {x + w - t, y}, {t, h * 0.5F}, color);
    }
    if (segments[digit][2]) {
        drawLine(target, {x + w - t, y + h * 0.5F}, {t, h * 0.5F}, color);
    }
    if (segments[digit][3]) {
        drawLine(target, {x, y + h}, {w, t}, color);
    }
    if (segments[digit][4]) {
        drawLine(target, {x, y + h * 0.5F}, {t, h * 0.5F}, color);
    }
    if (segments[digit][5]) {
        drawLine(target, {x, y}, {t, h * 0.5F}, color);
    }
    if (segments[digit][6]) {
        drawLine(target, {x, y + h * 0.5F}, {w, t}, color);
    }
}

} // namespace

void DebugOverlay::render(
    sf::RenderTarget& target,
    const core::VisualizationSettings& visualization,
    const simulation::SimulationManager& simulations
) const
{
    const auto bounds = trailCheckboxBounds;
    const sf::Color border{45, 50, 58};
    const sf::Color enabled{45, 96, 160};
    const sf::Color disabled{138, 145, 154};

    sf::RectangleShape checkbox{{bounds.width, bounds.height}};
    checkbox.setPosition({bounds.left, bounds.top});
    checkbox.setFillColor(sf::Color{255, 255, 255, 230});
    checkbox.setOutlineColor(border);
    checkbox.setOutlineThickness(2.0F);
    target.draw(checkbox);

    if (visualization.showTrails) {
        sf::RectangleShape fill{{bounds.width - 10.0F, bounds.height - 10.0F}};
        fill.setPosition({bounds.left + 5.0F, bounds.top + 5.0F});
        fill.setFillColor(enabled);
        target.draw(fill);
    }

    const sf::Color iconColor = visualization.showTrails ? enabled : disabled;
    drawIconSegment(target, {bounds.left + 34.0F, bounds.top + 8.0F}, {bounds.left + 52.0F, bounds.top + 15.0F}, iconColor);
    drawIconSegment(target, {bounds.left + 52.0F, bounds.top + 15.0F}, {bounds.left + 70.0F, bounds.top + 8.0F}, iconColor);

    sf::CircleShape dot{3.0F};
    dot.setFillColor(iconColor);
    dot.setOrigin({3.0F, 3.0F});
    dot.setPosition({bounds.left + 34.0F, bounds.top + 8.0F});
    target.draw(dot);
    dot.setPosition({bounds.left + 52.0F, bounds.top + 15.0F});
    target.draw(dot);
    dot.setPosition({bounds.left + 70.0F, bounds.top + 8.0F});
    target.draw(dot);

    drawButtonBox(target, decrementLinkBounds, border);
    drawButtonBox(target, linkCounterBounds, border);
    drawButtonBox(target, incrementLinkBounds, border);

    const sf::Color controlColor{45, 50, 58};
    drawLine(
        target,
        {decrementLinkBounds.left + 6.0F, decrementLinkBounds.top + 11.0F},
        {decrementLinkBounds.width - 12.0F, 2.0F},
        controlColor
    );
    drawLine(
        target,
        {incrementLinkBounds.left + 6.0F, incrementLinkBounds.top + 11.0F},
        {incrementLinkBounds.width - 12.0F, 2.0F},
        controlColor
    );
    drawLine(
        target,
        {incrementLinkBounds.left + 11.0F, incrementLinkBounds.top + 6.0F},
        {2.0F, incrementLinkBounds.height - 12.0F},
        controlColor
    );

    drawDigit(target, static_cast<int>(simulations.primaryLinkCount()), linkCounterBounds, controlColor);
}

} // namespace pendulum::ui
