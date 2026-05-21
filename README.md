# Pendulum

Pendulum is a C++20/SFML real-time simulation sandbox for pendulum systems, chaos visualization, and future numerical analysis tooling.

The project is intentionally split into small layers:

- `core`: math and shared value types
- `physics`: deterministic simulation models and integrators
- `simulation`: worlds, lifecycle, and multi-simulation orchestration
- `render`: SFML visualization
- `input`: SFML event translation into simulation commands
- `ui`: debug and inspection overlays

Physics code does not depend on SFML.

## Build

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

Run:

```sh
./build/debug/pendulum_app
```

SFML 3.1.0 is fetched by CMake.

