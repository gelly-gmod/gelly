add_library(
        gelly_fluid_sim
        STATIC
        include/GellyFluidSim.h
        src/GellyFluidSim.cpp
        include/fluidsim/ISimContext.h include/fluidsim/CD3D11SimContext.h src/fluidsim/CD3D11SimContext.cpp include/fluidsim/IFluidSimulation.h include/fluidsim/ISimData.h include/fluidsim/CD3D11CPUSimData.h src/fluidsim/CD3D11CPUSimData.cpp include/fluidsim/CD3D11DebugFluidSimulation.h src/fluidsim/CD3D11DebugFluidSimulation.cpp)

target_include_directories(
        gelly_fluid_sim
        PUBLIC
        include
        ../gelly-interfaces/include
        ../gelly-d3d/include
)

target_link_libraries(
        gelly_fluid_sim
        PUBLIC
        gelly_d3d9
)