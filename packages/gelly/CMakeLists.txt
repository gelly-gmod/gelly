cmake_minimum_required(VERSION 3.25)
project(gelly CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${GELLY_ROOT_DIR}/cmake")

add_subdirectory(../gsc "gsc")
add_subdirectory(../gbp "gbp")
set(GBP_PATH "${GBP_PATH}" PARENT_SCOPE)
set(GSC_PATH "${GSC_PATH}" PARENT_SCOPE)

add_subdirectory(modules/gelly-d3d)
add_subdirectory(modules/gelly-fluid-sim)
add_subdirectory(modules/gelly-fluid-renderer)

# Propagate flex dlls up
set(FLEX_DLLS ${FLEX_DLLS} PARENT_SCOPE)

set(GELLY_INCLUDE_DIRS
        modules/gelly-fluid-sim/include
        modules/gelly-fluid-renderer/include
        modules/gelly-d3d/include
        PARENT_SCOPE
)
set(GELLY_LIBS
        gelly_fluid_sim
        gelly_fluid_renderer
        gelly_d3d9
        PARENT_SCOPE
)

