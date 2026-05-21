# Architecture

Pendulum is organized around a strict separation between simulation, rendering, input, orchestration, and future UI/debug tooling.

## Dependency Direction

```text
app
  -> input
  -> render
  -> ui
  -> simulation
  -> physics
  -> core
```

`physics` depends only on `core`. It must not include SFML headers or rendering-specific concepts.

## Main Concepts

- `DynamicalSystem`: pure physics object that exposes state and derivatives.
- `Integrator`: numerical method that advances a `DynamicalSystem`.
- `PendulumSystem`: pendulum-specific system that can produce render/analysis snapshots.
- `SimulationWorld`: independent simulation timeline with its own systems and integrator.
- `SimulationManager`: owns multiple worlds.
- `Renderer`: draws snapshots with SFML.
- `InputController`: translates platform/window events into actions.

## Update Model

Simulation uses a fixed timestep. Rendering is allowed to run at a variable frame rate.

```text
poll input
accumulate elapsed time
while accumulator >= fixedDt:
  apply commands
  update simulations by fixedDt
render latest snapshots
```

