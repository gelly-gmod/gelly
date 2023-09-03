add_subdirectory(vendor/gmod-module-base)
add_subdirectory(vendor/minhook)
add_subdirectory(vendor/GMFS)

add_library(gelly_binary_module SHARED
        src/main.cpp src/TextureOverride.h src/TextureOverride.cpp src/Gelly.h src/Gelly.cpp src/d3d9/D3D9Shader.h src/d3d9/D3D9Shader.cpp src/d3d9/ErrorHandling.h)
target_include_directories(gelly_binary_module PRIVATE
        vendor/gmod-module-base/include
        vendor/minhook/include
        vendor/GMFS
        ../gelly-engine/include
        ../gelly-renderer/include
        )
target_link_libraries(gelly_binary_module PRIVATE minhook gelly_engine gelly_renderer GMFS d3d9.lib dxguid.lib)
# The dlls are named gmcl_<module_name>_<arch>.dll
if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
    set_target_properties(gelly_binary_module PROPERTIES OUTPUT_NAME gmcl_gelly_win64)
elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86")
    set_target_properties(gelly_binary_module PROPERTIES OUTPUT_NAME gmcl_gelly_win32)
endif ()

function(add_shader shader_name)
    message(STATUS "Adding shader ${shader_name}")
    set(shader_path src/d3d9/shaders/${shader_name}.hlsl)
    set(output_path ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/${shader_name}.embed.hlsl)

    # Prepares the shader for embedding by adding R"===()===" around the shader code
    file(READ ${shader_path} shader_code)
    set(shader_code "R\"===(${shader_code})===\"")
    file(WRITE ${output_path} "${shader_code}")
endfunction()

add_shader(CompositeVS)
add_shader(CompositePS)