include(GetFleXArchitecture)

add_library(
        gelly_fluid_sim
        STATIC
        src/v2/simulation.cpp
        src/v2/simulation.h
        src/v2/solver.cpp
        src/v2/solver.h
        src/v2/helpers/flex-buffer.h
        src/v2/helpers/flex-gpu-buffer.h
        src/v2/scene/scene.cpp
        src/v2/scene/scene.h
        src/v2/scene/object.h
        src/v2/scene/object-handler.h
        src/v2/scene/helpers/flex-types.h
        src/v2/scene/helpers/monotonic-counter.h
        src/v2/scene/handlers/shape-handler.h
        src/v2/scene/handlers/shape-handler.cpp
        src/v2/scene/handlers/forcefield-handler.h
        src/v2/scene/handlers/forcefield-handler.cpp
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
        src
        include
        ../gelly-interfaces/include
        ../gelly-fluid-renderer/src
        ${FLEX_PATH}/include
        vendor/DirectXMath/Inc
)

target_link_libraries(
        gelly_fluid_sim
        PUBLIC
        ${FLEX_LIBS}
)
