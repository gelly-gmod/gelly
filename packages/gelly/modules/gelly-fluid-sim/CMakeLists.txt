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
        src/fluidsim/rtfr/Dataset.cpp
        src/fluidsim/rtfr/Dataset.h
        src/fluidsim/CD3D11RTFRFluidSimulation.cpp
        include/fluidsim/CD3D11RTFRFluidSimulation.h
        include/fluidsim/ISimCommandList.h
        src/fluidsim/CSimpleSimCommandList.cpp
        include/fluidsim/CSimpleSimCommandList.h
        include/fluidsim/CD3D11FlexFluidSImulation.h
        src/fluidsim/CD3D11FlexFluidSimulation.cpp
        include/fluidsim/ISimRigids.h
        include/fluidsim/CFlexSimRigids.h
        src/fluidsim/CFlexSimRigids.cpp
        include/fluidsim/scene/scene.cpp
        include/fluidsim/scene/scene.h
        include/fluidsim/scene/object.h
        include/fluidsim/scene/object-handler.h
        include/fluidsim/scene/handlers/shape-handler.h
        include/fluidsim/scene/helpers/monotonic-counter.h
        include/fluidsim/scene/handlers/shape-handler.cpp
        include/fluidsim/scene/helpers/flex-types.h
        include/fluidsim/scene/handlers/forcefield-handler.cpp
        include/fluidsim/scene/handlers/forcefield-handler.h
        src/v2/simulation.cpp
        src/v2/simulation.h
        src/v2/solver.cpp
        src/v2/solver.h
        src/v2/helpers/flex-buffer.h
        src/v2/helpers/flex-gpu-buffer.h
)

GetFleXArchitecture()

# need to propagate upwards where the libs are located, and where the dlls are located
set(FLEX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/FleX")

set(FLEX_LIBS
        # The debug versions provide virtually no additional information, and are not worth the extra 20 milliseconds of sim time
        "${FLEX_PATH}/lib/${ARCH}/NvFlexReleaseD3D_${ARCH_SUFFIX}.lib"
        "${FLEX_PATH}/lib/${ARCH}/NvFlexDeviceRelease_${ARCH_SUFFIX}.lib"
        "${FLEX_PATH}/lib/${ARCH}/NvFlexExtReleaseD3D_${ARCH_SUFFIX}.lib"
)

# The user needs to copy these dlls into the executable directory
set(FLEX_DLLS
        "${FLEX_PATH}/bin/${ARCH}/NvFlexReleaseD3D_${ARCH_SUFFIX}.dll"
        "${FLEX_PATH}/bin/${ARCH}/NvFlexDeviceRelease_${ARCH_SUFFIX}.dll"
        "${FLEX_PATH}/bin/${ARCH}/NvFlexExtReleaseD3D_${ARCH_SUFFIX}.dll"
        "${FLEX_PATH}/bin/${ARCH}/nvToolsExt64_1.dll"
        "${FLEX_PATH}/bin/${ARCH}/amd_ags_x64.dll"
        "${FLEX_PATH}/bin/${ARCH}/GFSDK_Aftermath_Lib.x64.dll"
        PARENT_SCOPE
)

# Propagate GELLY_ENABLE_RENDERDOC_CAPTURES down to compile definitions
if (GELLY_ENABLE_RENDERDOC_CAPTURES)
    add_compile_definitions("GELLY_ENABLE_RENDERDOC_CAPTURES")
endif ()


target_include_directories(
        gelly_fluid_sim
        PUBLIC
        src/fluidsim
        src
        include
        ../gelly-interfaces/include
        ${FLEX_PATH}/include
        vendor/DirectXMath/Inc
)


target_link_libraries(
        gelly_fluid_sim
        PUBLIC
        ${FLEX_LIBS}
)
