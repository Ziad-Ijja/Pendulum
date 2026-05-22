#include "pendulum/simulation/simulation_world.hpp"

#include <cmath>
#include <random>
#include <stdexcept>
#include <utility>

namespace pendulum::simulation {

SimulationWorld::SimulationWorld(std::string name)
    : name_(std::move(name))
{
}

void SimulationWorld::setIntegrator(std::unique_ptr<physics::Integrator> integrator)
{
    if (!integrator) {
        throw std::invalid_argument{"SimulationWorld integrator cannot be null"};
    }

    integrator_ = std::move(integrator);
}

void SimulationWorld::addSystem(std::unique_ptr<physics::PendulumSystem> system)
{
    if (!system) {
        throw std::invalid_argument{"SimulationWorld system cannot be null"};
    }

    systems_.push_back(std::move(system));
    initialStates_.push_back(systems_.back()->state());
    trails_.emplace_back();
    const auto snapshot = systems_.back()->snapshot(time_);
    if (!snapshot.masses.empty()) {
        trails_.back().push(snapshot.masses.back());
    }
    recordEnergySample(true);
}

void SimulationWorld::update(double dt)
{
    if (paused_) {
        return;
    }
    if (!integrator_) {
        throw std::logic_error{"SimulationWorld cannot update without an integrator"};
    }

    for (auto& system : systems_) {
        integrator_->step(*system, time_, dt);
    }

    time_ += dt;
    recordTrailSamples();
    recordEnergySample(false);
}

void SimulationWorld::pause()
{
    paused_ = true;
}

void SimulationWorld::resume()
{
    paused_ = false;
}

void SimulationWorld::reset()
{
    for (std::size_t i = 0; i < systems_.size(); ++i) {
        if (i < initialStates_.size()) {
            systems_[i]->setState(initialStates_[i]);
        }
    }

    resetTime();
}

void SimulationWorld::resetTime()
{
    time_ = 0.0;
    clearTrails();
    resetEnergyHistory();
}

void SimulationWorld::clearTrails()
{
    for (std::size_t i = 0; i < trails_.size(); ++i) {
        resetTrailForSystem(i);
    }
}

void SimulationWorld::randomizeAngles()
{
    static std::mt19937 engine{std::random_device{}()};
    constexpr double pi = 3.14159265358979323846;
    std::uniform_real_distribution<double> angleDistribution{-pi, pi};

    for (std::size_t i = 0; i < systems_.size(); ++i) {
        auto state = systems_[i]->state();
        const std::size_t linkCount = systems_[i]->linkCount();
        if (state.size() < linkCount) {
            continue;
        }

        for (std::size_t angleIndex = 0; angleIndex < linkCount; ++angleIndex) {
            state[angleIndex] = angleDistribution(engine);
        }
        systems_[i]->setState(state);
        saveCurrentStateAsInitial(i);
    }

    resetTime();
}

void SimulationWorld::resetVelocities()
{
    for (std::size_t i = 0; i < systems_.size(); ++i) {
        auto state = systems_[i]->state();
        const std::size_t linkCount = systems_[i]->linkCount();
        if (state.size() < linkCount * 2) {
            continue;
        }

        for (std::size_t velocityIndex = linkCount; velocityIndex < linkCount * 2; ++velocityIndex) {
            state[velocityIndex] = 0.0;
        }
        systems_[i]->setState(state);
        saveCurrentStateAsInitial(i);
    }

    resetTime();
}

void SimulationWorld::setPaused(bool paused)
{
    paused_ = paused;
}

std::optional<DragHandle> SimulationWorld::findDraggableMass(
    std::size_t worldIndex,
    core::Vec2 worldPoint,
    double maxDistance
) const
{
    for (std::size_t systemIndex = 0; systemIndex < systems_.size(); ++systemIndex) {
        const auto massIndex = systems_[systemIndex]->nearestMass(worldPoint, maxDistance);
        if (massIndex.has_value()) {
            return DragHandle{worldIndex, systemIndex, *massIndex};
        }
    }

    return std::nullopt;
}

bool SimulationWorld::dragMassWithoutVelocity(const DragHandle& handle, core::Vec2 worldPoint)
{
    if (handle.systemIndex >= systems_.size()) {
        return false;
    }

    const bool moved = systems_[handle.systemIndex]->setMassPositionWithoutVelocity(handle.massIndex, worldPoint);
    if (moved && handle.systemIndex < trails_.size()) {
        const auto snapshot = systems_[handle.systemIndex]->snapshot(time_);
        if (!snapshot.masses.empty()) {
            trails_[handle.systemIndex].push(snapshot.masses.back());
        }
        recordEnergySample(true);
    }
    return moved;
}

std::size_t SimulationWorld::primaryLinkCount() const
{
    if (systems_.empty()) {
        return 0;
    }

    return systems_.front()->linkCount();
}

bool SimulationWorld::addLinkToPrimarySystem(physics::PendulumLink link)
{
    if (systems_.empty()) {
        return false;
    }

    const bool added = systems_.front()->addLinkAfterLast(link);
    if (added) {
        saveCurrentStateAsInitial(0);
        resetTrailForSystem(0);
        resetEnergyHistory();
    }
    return added;
}

bool SimulationWorld::removeLinkFromPrimarySystem()
{
    if (systems_.empty()) {
        return false;
    }

    const bool removed = systems_.front()->removeLastLink();
    if (removed) {
        saveCurrentStateAsInitial(0);
        resetTrailForSystem(0);
        resetEnergyHistory();
    }
    return removed;
}

bool SimulationWorld::isPaused() const noexcept
{
    return paused_;
}

double SimulationWorld::time() const noexcept
{
    return time_;
}

const std::string& SimulationWorld::name() const noexcept
{
    return name_;
}

const std::vector<TrajectoryBuffer>& SimulationWorld::trails() const noexcept
{
    return trails_;
}

const EnergyHistory& SimulationWorld::energyHistory() const noexcept
{
    return energyHistory_;
}

std::vector<physics::PendulumSnapshot> SimulationWorld::snapshots() const
{
    std::vector<physics::PendulumSnapshot> result;
    result.reserve(systems_.size());

    for (const auto& system : systems_) {
        result.push_back(system->snapshot(time_));
    }

    return result;
}

void SimulationWorld::recordTrailSamples()
{
    for (std::size_t i = 0; i < systems_.size(); ++i) {
        const auto snapshot = systems_[i]->snapshot(time_);
        if (!snapshot.masses.empty()) {
            trails_[i].push(snapshot.masses.back());
        }
    }
}

void SimulationWorld::recordEnergySample(bool resetBaseline)
{
    physics::EnergySample aggregate;
    aggregate.time = time_;

    for (const auto& system : systems_) {
        const auto sample = system->energy(time_);
        aggregate.kinetic += sample.kinetic;
        aggregate.potential += sample.potential;
    }

    aggregate.total = aggregate.kinetic + aggregate.potential;
    if (resetBaseline || !hasEnergyBaseline_) {
        energyBaseline_ = aggregate.total;
        hasEnergyBaseline_ = true;
    }
    aggregate.drift = aggregate.total - energyBaseline_;
    energyHistory_.push(aggregate);
}

void SimulationWorld::resetTrailForSystem(std::size_t systemIndex)
{
    if (systemIndex >= systems_.size() || systemIndex >= trails_.size()) {
        return;
    }

    trails_[systemIndex].clear();
    const auto snapshot = systems_[systemIndex]->snapshot(time_);
    if (!snapshot.masses.empty()) {
        trails_[systemIndex].push(snapshot.masses.back());
    }
}

void SimulationWorld::resetEnergyHistory()
{
    energyHistory_.clear();
    recordEnergySample(true);
}

void SimulationWorld::saveCurrentStateAsInitial(std::size_t systemIndex)
{
    if (systemIndex >= systems_.size()) {
        return;
    }
    if (systemIndex >= initialStates_.size()) {
        initialStates_.resize(systemIndex + 1);
    }

    initialStates_[systemIndex] = systems_[systemIndex]->state();
}

} // namespace pendulum::simulation
