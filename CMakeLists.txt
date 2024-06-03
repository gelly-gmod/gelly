cmake_minimum_required(VERSION 3.25)
project(gelly-monorepo)

option(GELLY_BUILD_TESTBED "Build testbed" ON)
option(GELLY_BUILD_GMOD "Build GMod binary module" OFF)
option(GELLY_BUILD_CPUVISUALIZER "Build CPU Visualizer" OFF)
option(GELLY_PRODUCTION_BUILD "Build in production mode" OFF)

set(GELLY_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "Configuring Gelly with CMake version ${CMAKE_VERSION}")

if (GELLY_PRODUCTION_BUILD)
    message(STATUS "Building in production mode")
    add_compile_definitions(PRODUCTION_BUILD)
endif ()

if (GELLY_BUILD_TESTBED)
    add_subdirectory(packages/testbed)
endif ()

if (GELLY_BUILD_GMOD)
    add_subdirectory(packages/gelly-gmod)
endif ()

if (GELLY_BUILD_CPUVISUALIZER)
    # Need to switch GELLY_ENABLE_CPU_VISUALIZER to ON for this,
    # in the normal usage this will be OFF and there will be no
    # executable generated, and it'll fallback to just being a library
    set(GELLY_ENABLE_CPU_VISUALIZER FORCE CACHE BOOL "Enable CPU Visualizer" ON)
    add_subdirectory(packages/gelly/modules/gelly-cpu-refs)
endif ()