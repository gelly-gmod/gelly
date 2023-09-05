include(GetFleXArchitecture)

GetFleXArchitecture()

# Copy the FleX libraries to the build directory
set(FLEX_DLL_PATHS
        "${FLEX_PATH}/bin/${ARCH}/NvFlexReleaseD3D_${ARCH_SUFFIX}.dll"
        "${FLEX_PATH}/bin/${ARCH}/NvFlexDeviceRelease_${ARCH_SUFFIX}.dll"
        "${FLEX_PATH}/bin/${ARCH}/NvFlexExtReleaseD3D_${ARCH_SUFFIX}.dll"
        "${FLEX_PATH}/bin/${ARCH}/amd_ags_${ARCH_SUFFIX}.dll"
        "${FLEX_PATH}/bin/${ARCH}/GFSDK_Aftermath_Lib.${ARCH_SUFFIX}.dll"
        )

message(STATUS "Copying FleX DLLs to ${RELEASE_DIR}")
foreach (FLEX_DLL_PATH ${FLEX_DLL_PATHS})
    get_filename_component(FLEX_DLL_NAME ${FLEX_DLL_PATH} NAME)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FLEX_DLL_PATH} "${RELEASE_DIR}/${FLEX_DLL_NAME}")
endforeach ()