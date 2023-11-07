cmake_minimum_required(VERSION 3.25)
project(gelly CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
set(FLEX_PATH "${CMAKE_SOURCE_DIR}/modules/gelly-engine/vendor/FleX")

add_subdirectory(modules/gelly-d3d)
add_subdirectory(modules/gelly-shaders)
add_subdirectory(modules/gelly-fluid-sim)
add_subdirectory(modules/gelly-fluid-renderer)

function(generate_gelly_config)
    set(GELLY_INCLUDE_DIRS
            modules/gelly-fluid-sim/include
            modules/gelly-fluid-renderer/include
            modules/gelly-shaders/include # Don't use. Remove later
            modules/gelly-d3d/include
            PARENT_SCOPE
            )
    set(GELLY_LIBS
            gelly_fluid_sim
            gelly_fluid_renderer
            gelly_d3d9
            PARENT_SCOPE
            )
endfunction()

option(GELLY_BUILD_TESTBED "Build Testbed" ON)

if (GELLY_BUILD_TESTBED)
    add_subdirectory(testbed)
endif ()