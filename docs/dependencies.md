# Dependencies

## SFML

The project currently uses SFML 3.1.0 through CMake `FetchContent`.

SFML is isolated to:

- `render`
- `input`
- `ui`
- `app`

Physics and simulation code should remain independent of SFML.

## Future Options

The project can later move dependency management to vcpkg or system packages without changing the physics layer.

