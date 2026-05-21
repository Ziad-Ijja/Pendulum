#include "pendulum/simulation/trajectory_buffer.hpp"

namespace pendulum::simulation {

TrajectoryBuffer::TrajectoryBuffer(std::size_t capacity)
    : capacity_(capacity)
{
}

void TrajectoryBuffer::push(core::Vec2 point)
{
    if (capacity_ == 0) {
        return;
    }

    samples_.push_back(point);
    while (samples_.size() > capacity_) {
        samples_.pop_front();
    }
}

void TrajectoryBuffer::clear()
{
    samples_.clear();
}

const std::deque<core::Vec2>& TrajectoryBuffer::samples() const noexcept
{
    return samples_;
}

std::size_t TrajectoryBuffer::capacity() const noexcept
{
    return capacity_;
}

} // namespace pendulum::simulation

