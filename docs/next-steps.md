# Next Steps

The current project is a good base: it can run a chained pendulum, add/remove links, drag bobs without injecting mouse velocity, and toggle trajectory trails.

The next work should move the project from an interactive demo toward a scientific chaos laboratory.

## Recommended Order

### 1. Energy Visualization

Add energy measurement and a small real-time graph.

- total energy
- kinetic energy
- potential energy
- energy drift over time

Why this matters:

- shows whether the numerical integrator behaves well
- makes RK4 quality visible
- explains how dragging upward adds potential energy
- prepares the project for integrator comparison and chaos analysis

Status: implemented as a compact overlay graph with total, kinetic, potential, and drift traces.

### 2. Reset And Randomize Controls

Add controls for quick experimentation.

- reset current simulation
- clear trail
- randomize angles
- reset velocities to zero

Useful inputs:

- `R`: reset
- `C`: clear trail
- randomize button in the overlay

### 3. Integrator Switching

Allow switching between integrators at runtime.

- Euler
- RK4

Why this matters:

- Euler is useful as a visible baseline
- RK4 should preserve behavior much better
- energy drift can show the difference clearly

### 4. Side-By-Side Simulations

Add multiple viewports and run multiple simulations at once.

Good first comparisons:

- Euler vs RK4 with the same initial state
- two RK4 simulations with tiny initial angle differences
- different damping values
- different gravity values

This is one of the most important steps toward chaos visualization.

### 5. Speed-Colored Trails

Improve trails so they encode information.

Instead of only fading by age:

- blue = slow
- red = fast
- opacity = recency

This makes the trail useful as analysis, not just decoration.

### 6. Pause And Single-Step

Add deterministic stepping while paused.

- `Space`: pause/unpause
- `.` or right arrow: advance one fixed timestep

This helps debug physics, rendering, and interaction.

### 7. Parameter Controls

Expose important simulation parameters.

- gravity
- damping
- timestep
- link length
- link mass

Start with simple keyboard controls or overlay buttons before building a larger UI system.

### 8. Chaos Analysis Tools

Once side-by-side simulations exist, add real chaos tools.

- divergence between nearby simulations
- distance over time graph
- approximate Lyapunov exponent
- initial-condition presets
- simulation recording and replay

## Performance Notes

Before adding multithreading, prioritize:

- running the release build
- reducing avoidable allocations in chain dynamics
- batching render work
- limiting expensive trail drawing
- adding a debug timing overlay

Multithreading becomes valuable when running many independent simulations, not for one small chained pendulum.

## Preferred Near-Term Milestone

The best next milestone is:

```text
Energy graph + integrator switch + reset controls
```

That milestone turns the app into a useful numerical-methods sandbox and prepares it for side-by-side chaos experiments.
