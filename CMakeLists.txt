cmake_minimum_required(VERSION 3.25)
project(gelly-monorepo)

option(GELLY_BUILD_GMOD "Build GMod binary module" OFF)
option(GELLY_PRODUCTION_BUILD "Build in production mode" OFF)
option(GELLY_USE_DEBUG_LAYER "Build Gelly with D3D11 Debug Layer enabled" OFF)

set(GELLY_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${GELLY_ROOT_DIR}/cmake")

include(GetCurrentVersion)
include(GetFleXDeps)

get_current_version_from_changelog(${GELLY_ROOT_DIR}/CHANGELOG.md)
message(STATUS "Configuring Gelly with CMake version ${CMAKE_VERSION} and Gelly version ${GELLY_VERSION}")

get_flex_dependencies(${GELLY_ROOT_DIR}/packages/gelly/modules/gelly-fluid-sim/vendor/FleX)

find_program(SCCACHE sccache)
if (SCCACHE)
    message(STATUS "Using sccache for compilation")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${SCCACHE}")
    set(CMAKE_C_COMPILER_LAUNCHER "${SCCACHE}")
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded)
    cmake_policy(SET CMP0141 NEW)
endif ()

find_program(RCEDIT rcedit-x64)
if (RCEDIT)
    message(STATUS "Using rcedit for binary versioning")
endif ()

# if we use clang we gotta turn off -Wattributes, it can cause an absolutely absurd amount of warnings (200+)
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-attributes)
endif ()

if (GELLY_PRODUCTION_BUILD)
    message(STATUS "Building in production mode")
    add_compile_definitions(PRODUCTION_BUILD)
endif ()

if (GELLY_USE_DEBUG_LAYER)
    message(STATUS "Building with D3D11 Debug Layer enabled")
    add_compile_definitions(GELLY_USE_DEBUG_LAYER)
endif ()

if (GELLY_BUILD_GMOD)
    add_subdirectory(packages/gelly-gmod)
endif ()