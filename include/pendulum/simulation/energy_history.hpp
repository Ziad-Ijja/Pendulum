#pragma once

#include "pendulum/physics/energy_sample.hpp"

#include <cstddef>
#include <deque>

namespace pendulum::simulation {

class EnergyHistory {
public:
    explicit EnergyHistory(std::size_t capacity = 1800);

    void push(physics::EnergySample sample);
    void clear();

    [[nodiscard]] const std::deque<physics::EnergySample>& samples() const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;

private:
    std::deque<physics::EnergySample> samples_;
    std::size_t capacity_ = 1800;
};

} // namespace pendulum::simulation

