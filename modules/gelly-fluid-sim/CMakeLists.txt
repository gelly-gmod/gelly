include(GetFleXArchitecture)

add_library(
        gelly_fluid_sim
        STATIC
        include/GellyFluidSim.h
        src/GellyFluidSim.cpp
        include/fluidsim/ISimContext.h include/fluidsim/CD3D11SimContext.h src/fluidsim/CD3D11SimContext.cpp include/fluidsim/IFluidSimulation.h include/fluidsim/ISimData.h include/fluidsim/CD3D11CPUSimData.h src/fluidsim/CD3D11CPUSimData.cpp include/fluidsim/CD3D11DebugFluidSimulation.h src/fluidsim/CD3D11DebugFluidSimulation.cpp
        src/fluidsim/rtfr/RTFRLoader.cpp
        src/fluidsim/rtfr/RTFRLoader.h
        src/fluidsim/rtfr/FrameParser.cpp
        src/fluidsim/rtfr/FrameParser.h
)

GetFleXArchitecture()

# need to propagate upwards where the libs are located, and where the dlls are located
set(FLEX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/FleX")

set(FLEX_LIBS
        # The debug versions provide virtually no additional information, and are not worth the extra 20 milliseconds of sim time
        "${FLEX_PATH}/lib/${ARCH}/NvFlexReleaseD3D_${ARCH_SUFFIX}.lib"
        "${FLEX_PATH}/lib/${ARCH}/NvFlexDeviceRelease_${ARCH_SUFFIX}.lib"
)

# The user needs to copy these dlls into the executable directory
set(FLEX_DLLS
        "${FLEX_PATH}/bin/${ARCH}/NvFlexReleaseD3D_${ARCH_SUFFIX}.dll"
        "${FLEX_PATH}/bin/${ARCH}/NvFlexDeviceRelease_${ARCH_SUFFIX}.dll"
        "${FLEX_PATH}/bin/${ARCH}/nvToolsExt64_1.dll"
        PARENT_SCOPE
)

target_include_directories(
        gelly_fluid_sim
        PUBLIC
        include
        ../gelly-interfaces/include
        ../gelly-d3d/include
        ${FLEX_PATH}/include
)

target_link_libraries(
        gelly_fluid_sim
        PUBLIC
        gelly_d3d9
        ${FLEX_LIBS}
)
