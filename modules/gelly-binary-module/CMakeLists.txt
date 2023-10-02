add_subdirectory(vendor/gmod-module-base)
add_subdirectory(vendor/minhook)
add_subdirectory(vendor/GMFS)

add_library(gelly_binary_module SHARED
        src/main.cpp src/TextureOverride.h src/TextureOverride.cpp src/Gelly.h src/Gelly.cpp src/source/Interface.h src/source/D3DDeviceWrapper.h src/source/D3DDeviceWrapper.cpp src/compositor/Compositor.cpp src/compositor/Compositor.h src/compositor/passes/Pass.cpp src/compositor/passes/Pass.h src/compositor/passes/Composite.cpp src/compositor/passes/Composite.h src/compositor/PassResources.h src/hooking/VTable.cpp src/hooking/VTable.h src/source/IBaseClientDLL.cpp src/source/IBaseClientDLL.h src/source/MathTypes.h src/source/CViewSetup.h src/source/IVRenderView.cpp src/source/IVRenderView.h src/source/IMaterialSystem.cpp src/source/IMaterialSystem.h src/source/CShaderAPIDX8.cpp src/source/CShaderAPIDX8.h src/source/CTexture.cpp src/source/CTexture.h src/hooking/Library.cpp src/hooking/Library.h)
target_include_directories(gelly_binary_module PRIVATE
        vendor/gmod-module-base/include
        vendor/minhook/include
        vendor/GMFS
        vendor/DirectXMath/Inc
        src
        ../gelly-engine/include
        ../gelly-renderer/include
        ../gelly-d3d9/include
        ../gelly-shaders/
        )
target_link_libraries(gelly_binary_module PRIVATE minhook gelly_d3d9 gelly_engine gelly_renderer GMFS d3d9.lib dxguid.lib)
# The dlls are named gmcl_<module_name>_<arch>.dll
if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
    set_target_properties(gelly_binary_module PROPERTIES OUTPUT_NAME gmcl_gelly_win64)
elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86")
    set_target_properties(gelly_binary_module PROPERTIES OUTPUT_NAME gmcl_gelly_win32)
endif ()

add_custom_command(
        TARGET gelly_binary_module
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        -DFLEX_PATH=${FLEX_PATH}
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH} # Passthrough since this variable isn't set when we run this CMake script.
        -DRELEASE_DIR=${CMAKE_BINARY_DIR}/release
        -DCMAKE_HOST_SYSTEM_PROCESSOR=${CMAKE_HOST_SYSTEM_PROCESSOR}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CopyFleXDLLs.cmake
        COMMENT "Copying FleX DLLs..."
)

set_target_properties(gelly_binary_module
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/release"
        )

add_dependencies(gelly_binary_module GellyD3D9Shaders)