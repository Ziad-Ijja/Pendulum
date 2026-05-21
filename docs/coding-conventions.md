# Coding Conventions

## C++

- Use C++20.
- Prefer clear value types for state and parameters.
- Prefer `std::unique_ptr` for polymorphic ownership.
- Avoid raw owning pointers.
- Use `double` in physics and convert to `float` only at the rendering boundary.
- Keep SFML out of `core`, `physics`, and `simulation`.

## Naming

- Types: `PascalCase`
- Functions and variables: `camelCase`
- Files: `snake_case.hpp` and `snake_case.cpp`
- Namespaces: `pendulum::core`, `pendulum::physics`, `pendulum::simulation`, `pendulum::render`

## Testing

Prioritize tests for:

- state vector math
- integrator behavior
- deterministic stepping
- model lifecycle
- energy drift regressions

