#include "pendulum/simulation/simulation_world.hpp"

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
    trails_.emplace_back();
    const auto snapshot = systems_.back()->snapshot(time_);
    if (!snapshot.masses.empty()) {
        trails_.back().push(snapshot.masses.back());
    }
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
}

void SimulationWorld::pause()
{
    paused_ = true;
}

void SimulationWorld::resume()
{
    paused_ = false;
}

void SimulationWorld::resetTime()
{
    time_ = 0.0;
    for (auto& trail : trails_) {
        trail.clear();
    }
    recordTrailSamples();
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
        resetTrailForSystem(0);
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
        resetTrailForSystem(0);
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

} // namespace pendulum::simulation
