cmake_minimum_required(VERSION 3.25)
project(gelly-monorepo)

option(GELLY_BUILD_TESTBED "Build testbed" ON)
option(GELLY_BUILD_GMOD "Build GMod binary module" OFF)

if (GELLY_BUILD_TESTBED)
    add_subdirectory(packages/testbed)
endif ()

if (GELLY_BUILD_GMOD)
    add_subdirectory(packages/gelly-gmod)
endif ()