cmake_minimum_required(VERSION 3.25)
project(gelly-monorepo)

OPTION(GELLY_BUILD_TESTBED "Build testbed" ON)

if (GELLY_BUILD_TESTBED)
    add_subdirectory(packages/testbed)
endif ()