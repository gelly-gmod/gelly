option(BUILD_GELLY_RENDERER_TEST "Build Gelly Renderer Test" OFF)

if (BUILD_GELLY_RENDERER_TEST)
    message(STATUS "Copying shaders to test directory")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test)
    add_subdirectory(test)
endif ()

function(add_shader shader_name)
    message(STATUS "Adding shader ${shader_name}")
    set(shader_path ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/shaders/${shader_name}.hlsl)
    set(output_path ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/${shader_name}.embed.hlsl)

    # Prepares the shader for embedding by adding R"===()===" around the shader code
    file(READ ${shader_path} shader_code)
    set(shader_code "R\"===(${shader_code})===\"")
    file(WRITE ${output_path} "${shader_code}")
endfunction()

add_library(gelly_renderer STATIC
        include/GellyRenderer.h
        src/GellyRenderer.cpp include/detail/ErrorHandling.h
        src/Camera.cpp
        src/rendering/Technique.h
        src/rendering/techniques/ParticleRendering.cpp
        src/rendering/techniques/ParticleRendering.h include/detail/DataTypes.h src/rendering/techniques/PerFrameCBuffer.h src/rendering/SSTechnique.cpp src/rendering/techniques/NormalSmoothing.cpp include/detail/GBuffer.h)

target_link_libraries(gelly_renderer PUBLIC gelly_d3d9 d3d9.lib d3d11.lib d3dcompiler.lib)
target_include_directories(gelly_renderer PRIVATE src include ../gelly-d3d9/include)
target_include_directories(gelly_renderer PUBLIC src include ../gelly-d3d9/include)

add_custom_target(
        GellyD3D11Shaders
        COMMAND ${CMAKE_COMMAND}
        -DSRC_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -DLOCAL_SHADER_PATH="src/shaders/shaders"
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH} # Passthrough since this variable isn't set when we run this CMake script.
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PreprocessShaders.cmake
        COMMENT "Preprocessing D3D11 shaders..."
)

add_dependencies(gelly_renderer GellyD3D11Shaders)