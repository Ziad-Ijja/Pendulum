# Numerical Methods

The simulation core is built around generic ODE integration.

State is represented as a flat `StateVector`. For a chain of N pendulum links, the recommended layout is:

```text
[theta0, theta1, ..., thetaN-1, omega0, omega1, ..., omegaN-1]
```

Initial integrators:

- Euler: simple baseline for comparison and debugging.
- RK4: default method for interactive simulation.

Future methods should implement the same `Integrator` interface:

- symplectic Euler
- velocity Verlet
- adaptive RK variants
- domain-specific constrained solvers

