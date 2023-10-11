option(BUILD_GELLYFLUIDSIM_TEST "Build GellyFluidSim test" OFF)

add_library(gelly_fluid_sim STATIC
        include/GellyFluid.h
        include/detail/PBFSolver.h
        )

target_include_directories(gelly_fluid_sim PUBLIC
        ../gelly-d3d/include
        include
        )
