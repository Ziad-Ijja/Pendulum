#pragma once

namespace pendulum::physics {

struct EnergySample {
    double kinetic = 0.0;
    double potential = 0.0;
    double total = 0.0;
    double drift = 0.0;
    double time = 0.0;
};

} // namespace pendulum::physics

