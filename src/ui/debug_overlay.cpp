#include "pendulum/ui/debug_overlay.hpp"

#include "pendulum/simulation/simulation_manager.hpp"
#include "pendulum/ui/control_layout.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Angle.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <deque>
#include <limits>
#include <string>
#include <string_view>

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

const sf::Font* overlayFont()
{
    static sf::Font font;
    static const bool loaded = [] {
        constexpr std::array<std::string_view, 3> paths{
            "/usr/share/fonts/TTF/DejaVuSans.ttf",
            "/usr/share/fonts/noto/NotoSans-Regular.ttf",
            "/usr/share/fonts/liberation/LiberationSans-Regular.ttf"
        };

        for (const auto path : paths) {
            if (font.openFromFile(std::string(path))) {
                return true;
            }
        }
        return false;
    }();

    return loaded ? &font : nullptr;
}

void drawText(
    sf::RenderTarget& target,
    std::string_view value,
    sf::Vector2f position,
    unsigned int size,
    sf::Color color
)
{
    const sf::Font* font = overlayFont();
    if (font == nullptr) {
        return;
    }

    sf::Text text{*font, std::string(value), size};
    text.setPosition(position);
    text.setFillColor(color);
    target.draw(text);
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

core::ScreenRect energyGraphBounds(sf::Vector2u targetSize)
{
    constexpr float margin = 18.0F;
    const float availableWidth = std::max(320.0F, static_cast<float>(targetSize.x) - margin * 2.0F);
    const float graphWidth = std::min(760.0F, availableWidth);
    const float graphHeight = std::min(240.0F, std::max(180.0F, static_cast<float>(targetSize.y) * 0.26F));

    return {
        margin,
        static_cast<float>(targetSize.y) - graphHeight - margin,
        graphWidth,
        graphHeight
    };
}

void drawGraphBackground(sf::RenderTarget& target, core::ScreenRect bounds, sf::Color border)
{
    sf::RectangleShape background{{bounds.width, bounds.height}};
    background.setPosition({bounds.left, bounds.top});
    background.setFillColor(sf::Color{255, 255, 255, 218});
    background.setOutlineColor(border);
    background.setOutlineThickness(1.0F);
    target.draw(background);

    const sf::Color grid{214, 218, 224, 150};
    for (int i = 1; i < 4; ++i) {
        const float y = bounds.top + bounds.height * static_cast<float>(i) / 4.0F;
        drawLine(target, {bounds.left, y}, {bounds.width, 1.0F}, grid);
    }
}

void drawLegendItem(
    sf::RenderTarget& target,
    sf::Vector2f position,
    sf::Color color,
    std::string_view label
)
{
    sf::RectangleShape swatch{{22.0F, 4.0F}};
    swatch.setPosition({position.x, position.y + 8.0F});
    swatch.setFillColor(color);
    target.draw(swatch);

    drawText(target, label, {position.x + 30.0F, position.y - 1.0F}, 14, sf::Color{45, 50, 58});
}

template <typename ValueSelector>
void drawEnergyTrace(
    sf::RenderTarget& target,
    const std::deque<physics::EnergySample>& samples,
    core::ScreenRect bounds,
    double minValue,
    double maxValue,
    sf::Color color,
    ValueSelector selectValue
)
{
    if (samples.size() < 2 || maxValue <= minValue) {
        return;
    }

    sf::VertexArray trace{sf::PrimitiveType::LineStrip, samples.size()};
    for (std::size_t i = 0; i < samples.size(); ++i) {
        const double value = selectValue(samples[i]);
        const double normalized = std::clamp((value - minValue) / (maxValue - minValue), 0.0, 1.0);
        const float x = bounds.left + bounds.width * static_cast<float>(i) / static_cast<float>(samples.size() - 1);
        const float y = bounds.top + bounds.height * static_cast<float>(1.0 - normalized);
        trace[i].position = {x, y};
        trace[i].color = color;
    }

    target.draw(trace);
}

void drawEnergyGraph(sf::RenderTarget& target, const simulation::SimulationManager& simulations, sf::Color border)
{
    const auto* history = simulations.primaryEnergyHistory();
    if (history == nullptr || history->samples().size() < 2) {
        return;
    }

    const auto bounds = energyGraphBounds(target.getSize());
    const float headerHeight = 32.0F;
    const auto plotBounds = core::ScreenRect{
        bounds.left + 10.0F,
        bounds.top + headerHeight + 8.0F,
        bounds.width - 20.0F,
        bounds.height - headerHeight - 18.0F
    };
    const auto topBounds = core::ScreenRect{plotBounds.left, plotBounds.top, plotBounds.width, plotBounds.height * 0.66F};
    const auto driftBounds = core::ScreenRect{
        plotBounds.left,
        plotBounds.top + plotBounds.height * 0.74F,
        plotBounds.width,
        plotBounds.height * 0.22F
    };
    const auto& samples = history->samples();

    drawGraphBackground(target, bounds, border);
    drawText(target, "Energy", {bounds.left + 10.0F, bounds.top + 7.0F}, 16, sf::Color{45, 50, 58});
    drawLegendItem(target, {bounds.left + 92.0F, bounds.top + 7.0F}, sf::Color{45, 96, 160}, "total");
    drawLegendItem(target, {bounds.left + 178.0F, bounds.top + 7.0F}, sf::Color{206, 70, 55}, "kinetic");
    drawLegendItem(target, {bounds.left + 282.0F, bounds.top + 7.0F}, sf::Color{35, 137, 126}, "potential");
    drawLegendItem(target, {bounds.left + 402.0F, bounds.top + 7.0F}, sf::Color{120, 82, 169}, "drift");

    double minEnergy = std::numeric_limits<double>::max();
    double maxEnergy = std::numeric_limits<double>::lowest();
    double maxAbsDrift = 1.0e-9;

    for (const auto& sample : samples) {
        minEnergy = std::min({minEnergy, sample.kinetic, sample.potential, sample.total});
        maxEnergy = std::max({maxEnergy, sample.kinetic, sample.potential, sample.total});
        maxAbsDrift = std::max(maxAbsDrift, std::abs(sample.drift));
    }

    if (std::abs(maxEnergy - minEnergy) < 1.0e-9) {
        maxEnergy += 1.0;
        minEnergy -= 1.0;
    }

    drawLine(
        target,
        {plotBounds.left, topBounds.top + topBounds.height + plotBounds.height * 0.04F},
        {plotBounds.width, 1.0F},
        sf::Color{184, 188, 196, 190}
    );

    drawEnergyTrace(target, samples, topBounds, minEnergy, maxEnergy, sf::Color{45, 96, 160}, [](const auto& s) {
        return s.total;
    });
    drawEnergyTrace(target, samples, topBounds, minEnergy, maxEnergy, sf::Color{206, 70, 55}, [](const auto& s) {
        return s.kinetic;
    });
    drawEnergyTrace(target, samples, topBounds, minEnergy, maxEnergy, sf::Color{35, 137, 126}, [](const auto& s) {
        return s.potential;
    });

    drawLine(
        target,
        {driftBounds.left, driftBounds.top + driftBounds.height * 0.5F},
        {driftBounds.width, 1.0F},
        sf::Color{184, 188, 196, 170}
    );
    drawEnergyTrace(
        target,
        samples,
        driftBounds,
        -maxAbsDrift,
        maxAbsDrift,
        sf::Color{120, 82, 169},
        [](const auto& s) {
            return s.drift;
        }
    );
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
    drawEnergyGraph(target, simulations, border);
}

} // namespace pendulum::ui
