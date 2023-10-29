cmake_minimum_required(VERSION 3.25)
project(gelly CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
set(FLEX_PATH "${CMAKE_SOURCE_DIR}/modules/gelly-engine/vendor/FleX")

add_subdirectory(modules/gelly-d3d)
add_subdirectory(modules/gelly-shaders)
add_subdirectory(modules/gelly-fluid-sim)
add_subdirectory(modules/gelly-fluid-renderer)