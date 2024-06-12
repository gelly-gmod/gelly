include(GetFleXArchitecture)

function(get_flex_dependencies flex_dir)
    GetFleXArchitecture()
    set(FLEX_LIBS
            ${flex_dir}/bin/${ARCH}/amd_ags_x64.dll
            ${flex_dir}/bin/${ARCH}/GFSDK_Aftermath_Lib.x64.dll
            ${flex_dir}/bin/${ARCH}/NvFlexReleaseD3D_${ARCH_SUFFIX}.dll
            ${flex_dir}/bin/${ARCH}/NvFlexExtReleaseD3D_${ARCH_SUFFIX}.dll
            ${flex_dir}/bin/${ARCH}/NvFlexDeviceRelease_${ARCH_SUFFIX}.dll
            ${flex_dir}/bin/${ARCH}/nvToolsExt64_1.dll
            PARENT_SCOPE)
endfunction()
