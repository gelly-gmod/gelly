option(BUILD_GELLYFLUIDSIM_TEST "Build GellyFluidSim test" OFF)

if (BUILD_GELLYFLUIDSIM_TEST)
    add_subdirectory(test)
endif ()

add_library(gelly_fluid_sim STATIC
        include/GellyFluid.h
        include/detail/PBFSolver.h
        src/SolverContext.cpp
        include/detail/MathPOD.h src/PBFSolver.cpp)

target_include_directories(gelly_fluid_sim PUBLIC
        ../gelly-d3d/include
        ../gelly-shaders/
        include
        )
