#include "pendulum/physics/chain_pendulum_system.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace pendulum::physics {

namespace {

using Matrix = std::vector<std::vector<double>>;

void validateLink(PendulumLink link)
{
    if (link.length <= 0.0) {
        throw std::invalid_argument{"Pendulum length must be positive"};
    }
    if (link.mass <= 0.0) {
        throw std::invalid_argument{"Pendulum mass must be positive"};
    }
}

[[nodiscard]] std::vector<double> suffixMasses(const std::vector<PendulumLink>& links)
{
    std::vector<double> suffix(links.size(), 0.0);
    double total = 0.0;
    for (std::size_t i = links.size(); i > 0; --i) {
        total += links[i - 1].mass;
        suffix[i - 1] = total;
    }
    return suffix;
}

[[nodiscard]] Matrix massMatrix(
    const std::vector<PendulumLink>& links,
    const std::vector<double>& angles,
    const std::vector<double>& suffix
)
{
    const std::size_t n = links.size();
    Matrix matrix(n, std::vector<double>(n, 0.0));

    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            const std::size_t massStart = std::max(i, j);
            matrix[i][j] = suffix[massStart] * links[i].length * links[j].length * std::cos(angles[i] - angles[j]);
        }
    }

    return matrix;
}

[[nodiscard]] double massMatrixDerivative(
    const std::vector<PendulumLink>& links,
    const std::vector<double>& angles,
    const std::vector<double>& suffix,
    std::size_t row,
    std::size_t column,
    std::size_t angleIndex
)
{
    if (row != angleIndex && column != angleIndex) {
        return 0.0;
    }
    if (row == column) {
        return 0.0;
    }

    const double factor =
        suffix[std::max(row, column)] * links[row].length * links[column].length * -std::sin(angles[row] - angles[column]);
    const double rowTerm = row == angleIndex ? 1.0 : 0.0;
    const double columnTerm = column == angleIndex ? 1.0 : 0.0;
    return factor * (rowTerm - columnTerm);
}

[[nodiscard]] std::vector<double> solveLinearSystem(Matrix matrix, std::vector<double> rhs)
{
    const std::size_t n = rhs.size();

    for (std::size_t pivot = 0; pivot < n; ++pivot) {
        std::size_t best = pivot;
        for (std::size_t row = pivot + 1; row < n; ++row) {
            if (std::abs(matrix[row][pivot]) > std::abs(matrix[best][pivot])) {
                best = row;
            }
        }

        if (std::abs(matrix[best][pivot]) < 1.0e-12) {
            throw std::runtime_error{"Chain pendulum mass matrix is singular"};
        }

        if (best != pivot) {
            std::swap(matrix[pivot], matrix[best]);
            std::swap(rhs[pivot], rhs[best]);
        }

        const double divisor = matrix[pivot][pivot];
        for (std::size_t column = pivot; column < n; ++column) {
            matrix[pivot][column] /= divisor;
        }
        rhs[pivot] /= divisor;

        for (std::size_t row = 0; row < n; ++row) {
            if (row == pivot) {
                continue;
            }

            const double factor = matrix[row][pivot];
            for (std::size_t column = pivot; column < n; ++column) {
                matrix[row][column] -= factor * matrix[pivot][column];
            }
            rhs[row] -= factor * rhs[pivot];
        }
    }

    return rhs;
}

} // namespace

ChainPendulumSystem::ChainPendulumSystem(
    std::vector<PendulumLink> links,
    PhysicsParameters parameters,
    std::vector<double> anglesRadians,
    std::vector<double> angularVelocities
)
    : links_(std::move(links))
    , parameters_(parameters)
{
    if (links_.empty()) {
        throw std::invalid_argument{"ChainPendulumSystem needs at least one link"};
    }
    for (const auto& link : links_) {
        validateLink(link);
    }
    if (anglesRadians.size() != links_.size() || angularVelocities.size() != links_.size()) {
        throw std::invalid_argument{"ChainPendulumSystem angles and velocities must match link count"};
    }

    state_ = StateVector(links_.size() * 2);
    for (std::size_t i = 0; i < links_.size(); ++i) {
        state_[i] = anglesRadians[i];
        state_[links_.size() + i] = angularVelocities[i];
    }
}

StateVector ChainPendulumSystem::state() const
{
    return state_;
}

void ChainPendulumSystem::setState(const StateVector& state)
{
    if (state.size() != links_.size() * 2) {
        throw std::invalid_argument{"ChainPendulumSystem state must contain N angles followed by N angular velocities"};
    }

    state_ = state;
}

StateVector ChainPendulumSystem::derivative(const StateVector& state, double /*time*/) const
{
    if (state.size() != links_.size() * 2) {
        throw std::invalid_argument{"ChainPendulumSystem derivative state size does not match link count"};
    }

    const std::size_t n = links_.size();
    const auto angles = anglesFromState(state);
    const auto angularVelocities = angularVelocitiesFromState(state);
    const auto suffix = suffixMasses(links_);
    const auto matrix = massMatrix(links_, angles, suffix);

    std::vector<double> generalizedForces(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        const double gravityGradient = parameters_.gravity * links_[i].length * suffix[i] * std::sin(angles[i]);
        double christoffel = 0.0;

        for (std::size_t j = 0; j < n; ++j) {
            for (std::size_t k = 0; k < n; ++k) {
                const double gamma = 0.5
                    * (massMatrixDerivative(links_, angles, suffix, i, j, k)
                       + massMatrixDerivative(links_, angles, suffix, i, k, j)
                       - massMatrixDerivative(links_, angles, suffix, j, k, i));
                christoffel += gamma * angularVelocities[j] * angularVelocities[k];
            }
        }

        generalizedForces[i] = -christoffel - gravityGradient - parameters_.damping * angularVelocities[i];
    }

    const std::vector<double> angularAccelerations = solveLinearSystem(matrix, generalizedForces);

    StateVector result(n * 2);
    for (std::size_t i = 0; i < n; ++i) {
        result[i] = angularVelocities[i];
        result[n + i] = angularAccelerations[i];
    }
    return result;
}

PendulumSnapshot ChainPendulumSystem::snapshot(double time) const
{
    const std::size_t n = links_.size();
    const auto angles = anglesFromState(state_);

    PendulumSnapshot result;
    result.anchors.push_back({0.0, 0.0});
    result.joints.reserve(n);
    result.masses.reserve(n);
    result.time = time;

    core::Vec2 current{0.0, 0.0};
    for (std::size_t i = 0; i < n; ++i) {
        result.joints.push_back(current);
        current = {
            current.x + links_[i].length * std::sin(angles[i]),
            current.y + links_[i].length * std::cos(angles[i])
        };
        result.masses.push_back(current);
    }

    return result;
}

EnergySample ChainPendulumSystem::energy(double time) const
{
    const std::size_t n = links_.size();
    const auto angles = anglesFromState(state_);
    const auto angularVelocities = angularVelocitiesFromState(state_);

    double kinetic = 0.0;
    double potential = 0.0;

    core::Vec2 position{0.0, 0.0};
    core::Vec2 velocity{0.0, 0.0};
    for (std::size_t i = 0; i < n; ++i) {
        position = {
            position.x + links_[i].length * std::sin(angles[i]),
            position.y + links_[i].length * std::cos(angles[i])
        };
        velocity = {
            velocity.x + links_[i].length * std::cos(angles[i]) * angularVelocities[i],
            velocity.y - links_[i].length * std::sin(angles[i]) * angularVelocities[i]
        };

        const double speedSquared = velocity.x * velocity.x + velocity.y * velocity.y;
        kinetic += 0.5 * links_[i].mass * speedSquared;
        potential += -links_[i].mass * parameters_.gravity * position.y;
    }

    return {kinetic, potential, kinetic + potential, 0.0, time};
}

std::size_t ChainPendulumSystem::linkCount() const
{
    return links_.size();
}

bool ChainPendulumSystem::setMassPositionWithoutVelocity(std::size_t massIndex, core::Vec2 worldPoint)
{
    if (massIndex >= links_.size()) {
        return false;
    }

    const auto currentSnapshot = snapshot(0.0);
    const core::Vec2 parent = massIndex == 0 ? currentSnapshot.anchors.front() : currentSnapshot.masses[massIndex - 1];
    const core::Vec2 relative = worldPoint - parent;
    if (core::length(relative) < 1.0e-6) {
        return false;
    }

    const std::size_t n = links_.size();
    state_[massIndex] = std::atan2(relative.x, relative.y);
    for (std::size_t i = massIndex; i < n; ++i) {
        state_[n + i] = 0.0;
    }
    return true;
}

bool ChainPendulumSystem::addLinkAfterLast(PendulumLink link)
{
    validateLink(link);

    const std::size_t oldCount = links_.size();
    const StateVector oldState = state_;
    links_.push_back(link);

    state_ = StateVector(links_.size() * 2);
    for (std::size_t i = 0; i < oldCount; ++i) {
        state_[i] = oldState[i];
        state_[links_.size() + i] = oldState[oldCount + i];
    }
    state_[oldCount] = 0.0;
    state_[links_.size() + oldCount] = 0.0;
    return true;
}

bool ChainPendulumSystem::removeLastLink()
{
    if (links_.size() <= 1) {
        return false;
    }

    const std::size_t oldCount = links_.size();
    const StateVector oldState = state_;
    links_.pop_back();

    state_ = StateVector(links_.size() * 2);
    for (std::size_t i = 0; i < links_.size(); ++i) {
        state_[i] = oldState[i];
        state_[links_.size() + i] = oldState[oldCount + i];
    }
    return true;
}

const std::vector<PendulumLink>& ChainPendulumSystem::links() const noexcept
{
    return links_;
}

std::vector<double> ChainPendulumSystem::anglesFromState(const StateVector& state) const
{
    std::vector<double> angles(links_.size(), 0.0);
    for (std::size_t i = 0; i < links_.size(); ++i) {
        angles[i] = state[i];
    }
    return angles;
}

std::vector<double> ChainPendulumSystem::angularVelocitiesFromState(const StateVector& state) const
{
    std::vector<double> velocities(links_.size(), 0.0);
    for (std::size_t i = 0; i < links_.size(); ++i) {
        velocities[i] = state[links_.size() + i];
    }
    return velocities;
}

} // namespace pendulum::physics
