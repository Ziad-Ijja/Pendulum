#pragma once

#include "pendulum/core/screen_rect.hpp"

#include <algorithm>

namespace pendulum::ui {

inline constexpr core::ScreenRect trailCheckboxBounds{16.0F, 16.0F, 24.0F, 24.0F};
inline constexpr core::ScreenRect decrementLinkBounds{104.0F, 16.0F, 24.0F, 24.0F};
inline constexpr core::ScreenRect linkCounterBounds{134.0F, 16.0F, 24.0F, 24.0F};
inline constexpr core::ScreenRect incrementLinkBounds{164.0F, 16.0F, 24.0F, 24.0F};
inline constexpr core::ScreenRect resetButtonBounds{210.0F, 16.0F, 62.0F, 24.0F};
inline constexpr core::ScreenRect clearTrailsButtonBounds{282.0F, 16.0F, 58.0F, 24.0F};
inline constexpr core::ScreenRect randomizeButtonBounds{350.0F, 16.0F, 82.0F, 24.0F};
inline constexpr core::ScreenRect resetVelocitiesButtonBounds{442.0F, 16.0F, 76.0F, 24.0F};

inline constexpr float graphPanelMargin = 18.0F;

[[nodiscard]] inline core::ScreenRect energyGraphBounds(float targetWidth, float targetHeight)
{
    const float availableWidth = std::max(320.0F, targetWidth - graphPanelMargin * 2.0F);
    const float graphWidth = std::min(760.0F, availableWidth);
    const float graphHeight = std::min(240.0F, std::max(180.0F, targetHeight * 0.26F));

    return {
        graphPanelMargin,
        targetHeight - graphHeight - graphPanelMargin,
        graphWidth,
        graphHeight
    };
}

[[nodiscard]] inline core::ScreenRect minimizedEnergyGraphBounds(float targetHeight)
{
    constexpr float panelWidth = 154.0F;
    constexpr float panelHeight = 42.0F;

    return {
        graphPanelMargin,
        targetHeight - panelHeight - graphPanelMargin,
        panelWidth,
        panelHeight
    };
}

[[nodiscard]] inline core::ScreenRect energyGraphPanelBounds(
    float targetWidth,
    float targetHeight,
    bool expanded
)
{
    return expanded ? energyGraphBounds(targetWidth, targetHeight) : minimizedEnergyGraphBounds(targetHeight);
}

[[nodiscard]] inline core::ScreenRect graphToggleButtonBounds(core::ScreenRect panelBounds)
{
    constexpr float buttonSize = 22.0F;

    return {
        panelBounds.left + panelBounds.width - buttonSize - 8.0F,
        panelBounds.top + 6.0F,
        buttonSize,
        buttonSize
    };
}

} // namespace pendulum::ui
