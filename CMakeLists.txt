cmake_minimum_required(VERSION 3.28)

project(
    Pendulum
    VERSION 0.1.0
    DESCRIPTION "Real-time chaos simulation laboratory for pendulum systems"
    LANGUAGES CXX
)

include(FetchContent)
include(CTest)

option(PENDULUM_BUILD_TESTS "Build Pendulum tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

add_library(pendulum_project_options INTERFACE)
target_compile_features(pendulum_project_options INTERFACE cxx_std_20)

if(MSVC)
    target_compile_options(pendulum_project_options INTERFACE /W4 /permissive-)
else()
    target_compile_options(pendulum_project_options INTERFACE -Wall -Wextra -Wpedantic)
endif()

add_library(pendulum_core
    src/core/vec2.cpp
)
add_library(Pendulum::Core ALIAS pendulum_core)
target_include_directories(pendulum_core PUBLIC include)
target_link_libraries(pendulum_core PUBLIC pendulum_project_options)

add_library(pendulum_physics
    src/physics/chain_pendulum_system.cpp
    src/physics/euler_integrator.cpp
    src/physics/pendulum_system.cpp
    src/physics/rk4_integrator.cpp
    src/physics/single_pendulum_system.cpp
    src/physics/state_vector.cpp
)
add_library(Pendulum::Physics ALIAS pendulum_physics)
target_include_directories(pendulum_physics PUBLIC include)
target_link_libraries(pendulum_physics PUBLIC Pendulum::Core)

add_library(pendulum_simulation
    src/simulation/energy_history.cpp
    src/simulation/simulation_manager.cpp
    src/simulation/simulation_world.cpp
    src/simulation/trajectory_buffer.cpp
)
add_library(Pendulum::Simulation ALIAS pendulum_simulation)
target_include_directories(pendulum_simulation PUBLIC include)
target_link_libraries(pendulum_simulation PUBLIC Pendulum::Physics)

add_library(pendulum_render
    src/render/camera2d.cpp
    src/render/pendulum_renderer.cpp
    src/render/renderer.cpp
)
add_library(Pendulum::Render ALIAS pendulum_render)
target_include_directories(pendulum_render PUBLIC include)
target_link_libraries(pendulum_render PUBLIC Pendulum::Simulation PRIVATE SFML::Graphics)

add_library(pendulum_input
    src/input/input_controller.cpp
)
add_library(Pendulum::Input ALIAS pendulum_input)
target_include_directories(pendulum_input PUBLIC include)
target_link_libraries(pendulum_input PUBLIC Pendulum::Render PRIVATE SFML::Window)

add_library(pendulum_ui
    src/ui/debug_overlay.cpp
)
add_library(Pendulum::Ui ALIAS pendulum_ui)
target_include_directories(pendulum_ui PUBLIC include)
target_link_libraries(pendulum_ui PUBLIC Pendulum::Simulation PRIVATE SFML::Graphics)

add_executable(pendulum_app
    src/main.cpp
)
target_link_libraries(pendulum_app
    PRIVATE
        Pendulum::Input
        Pendulum::Render
        Pendulum::Ui
        SFML::Graphics
)

if(PENDULUM_BUILD_TESTS AND BUILD_TESTING)
    add_executable(pendulum_physics_tests
        tests/physics/state_vector_tests.cpp
    )
    target_link_libraries(pendulum_physics_tests PRIVATE Pendulum::Physics)
    add_test(NAME pendulum_physics_tests COMMAND pendulum_physics_tests)
endif()
