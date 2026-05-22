#pragma once

#include "pendulum/physics/integrator.hpp"
#include "pendulum/physics/pendulum_snapshot.hpp"
#include "pendulum/physics/pendulum_system.hpp"
#include "pendulum/simulation/drag_handle.hpp"
#include "pendulum/simulation/energy_history.hpp"
#include "pendulum/simulation/trajectory_buffer.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace pendulum::simulation {

class SimulationWorld {
public:
    explicit SimulationWorld(std::string name);

    void setIntegrator(std::unique_ptr<physics::Integrator> integrator);
    void addSystem(std::unique_ptr<physics::PendulumSystem> system);

    void update(double dt);
    void pause();
    void resume();
    void reset();
    void resetTime();
    void clearTrails();
    void randomizeAngles();
    void resetVelocities();
    void setPaused(bool paused);
    [[nodiscard]] std::optional<DragHandle> findDraggableMass(
        std::size_t worldIndex,
        core::Vec2 worldPoint,
        double maxDistance
    ) const;
    bool dragMassWithoutVelocity(const DragHandle& handle, core::Vec2 worldPoint);
    [[nodiscard]] std::size_t primaryLinkCount() const;
    bool addLinkToPrimarySystem(physics::PendulumLink link);
    bool removeLinkFromPrimarySystem();

    [[nodiscard]] bool isPaused() const noexcept;
    [[nodiscard]] double time() const noexcept;
    [[nodiscard]] const std::string& name() const noexcept;
    [[nodiscard]] const std::vector<TrajectoryBuffer>& trails() const noexcept;
    [[nodiscard]] const EnergyHistory& energyHistory() const noexcept;
    [[nodiscard]] std::vector<physics::PendulumSnapshot> snapshots() const;

private:
    void recordTrailSamples();
    void recordEnergySample(bool resetBaseline);
    void resetTrailForSystem(std::size_t systemIndex);
    void resetEnergyHistory();
    void saveCurrentStateAsInitial(std::size_t systemIndex);

    std::string name_;
    std::vector<std::unique_ptr<physics::PendulumSystem>> systems_;
    std::vector<physics::StateVector> initialStates_;
    std::vector<TrajectoryBuffer> trails_;
    EnergyHistory energyHistory_;
    std::unique_ptr<physics::Integrator> integrator_;
    double energyBaseline_ = 0.0;
    double time_ = 0.0;
    bool hasEnergyBaseline_ = false;
    bool paused_ = false;
};

} // namespace pendulum::simulation
