add_library(
        gelly_fluid_sim
        STATIC
        include/GellyFluidSim.h
        src/GellyFluidSim.cpp
        include/fluidsim/ISimContext.h)

target_include_directories(
        gelly_fluid_sim
        PUBLIC
        include)