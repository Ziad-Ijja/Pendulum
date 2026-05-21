#pragma once

#include <cstddef>
#include <initializer_list>
#include <vector>

namespace pendulum::physics {

class StateVector {
public:
    StateVector() = default;
    explicit StateVector(std::size_t size);
    StateVector(std::initializer_list<double> values);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] double operator[](std::size_t index) const;
    double& operator[](std::size_t index);

    [[nodiscard]] const std::vector<double>& values() const noexcept;
    [[nodiscard]] std::vector<double>& values() noexcept;

private:
    std::vector<double> values_;
};

[[nodiscard]] StateVector operator+(const StateVector& lhs, const StateVector& rhs);
[[nodiscard]] StateVector operator-(const StateVector& lhs, const StateVector& rhs);
[[nodiscard]] StateVector operator*(const StateVector& lhs, double scalar);
[[nodiscard]] StateVector operator*(double scalar, const StateVector& rhs);
[[nodiscard]] StateVector operator/(const StateVector& lhs, double scalar);

} // namespace pendulum::physics

