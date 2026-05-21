#pragma once

namespace pendulum::core {

struct ScreenRect {
    float left = 0.0F;
    float top = 0.0F;
    float width = 0.0F;
    float height = 0.0F;

    [[nodiscard]] bool contains(float x, float y) const noexcept
    {
        return x >= left && x <= left + width && y >= top && y <= top + height;
    }
};

} // namespace pendulum::core

