#include "pendulum/simulation/energy_history.hpp"

namespace pendulum::simulation {

EnergyHistory::EnergyHistory(std::size_t capacity)
    : capacity_(capacity)
{
}

void EnergyHistory::push(physics::EnergySample sample)
{
    if (capacity_ == 0) {
        return;
    }

    samples_.push_back(sample);
    while (samples_.size() > capacity_) {
        samples_.pop_front();
    }
}

void EnergyHistory::clear()
{
    samples_.clear();
}

const std::deque<physics::EnergySample>& EnergyHistory::samples() const noexcept
{
    return samples_;
}

std::size_t EnergyHistory::capacity() const noexcept
{
    return capacity_;
}

} // namespace pendulum::simulation

