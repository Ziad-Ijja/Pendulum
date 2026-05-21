#include "pendulum/physics/state_vector.hpp"

#include <stdexcept>

namespace pendulum::physics {

namespace {

void validateSameSize(const StateVector& lhs, const StateVector& rhs)
{
    if (lhs.size() != rhs.size()) {
        throw std::invalid_argument{"StateVector size mismatch"};
    }
}

} // namespace

StateVector::StateVector(std::size_t size)
    : values_(size, 0.0)
{
}

StateVector::StateVector(std::initializer_list<double> values)
    : values_(values)
{
}

std::size_t StateVector::size() const noexcept
{
    return values_.size();
}

bool StateVector::empty() const noexcept
{
    return values_.empty();
}

double StateVector::operator[](std::size_t index) const
{
    return values_.at(index);
}

double& StateVector::operator[](std::size_t index)
{
    return values_.at(index);
}

const std::vector<double>& StateVector::values() const noexcept
{
    return values_;
}

std::vector<double>& StateVector::values() noexcept
{
    return values_;
}

StateVector operator+(const StateVector& lhs, const StateVector& rhs)
{
    validateSameSize(lhs, rhs);

    StateVector result(lhs.size());
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        result[i] = lhs[i] + rhs[i];
    }
    return result;
}

StateVector operator-(const StateVector& lhs, const StateVector& rhs)
{
    validateSameSize(lhs, rhs);

    StateVector result(lhs.size());
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        result[i] = lhs[i] - rhs[i];
    }
    return result;
}

StateVector operator*(const StateVector& lhs, double scalar)
{
    StateVector result(lhs.size());
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        result[i] = lhs[i] * scalar;
    }
    return result;
}

StateVector operator*(double scalar, const StateVector& rhs)
{
    return rhs * scalar;
}

StateVector operator/(const StateVector& lhs, double scalar)
{
    StateVector result(lhs.size());
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        result[i] = lhs[i] / scalar;
    }
    return result;
}

} // namespace pendulum::physics
