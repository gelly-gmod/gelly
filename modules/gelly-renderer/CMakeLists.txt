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
        include/detail/Shader.h src/Shader.cpp
        src/Camera.cpp
        src/rendering/Technique.h
        src/rendering/techniques/ParticleRendering.cpp
        src/rendering/techniques/ParticleRendering.h include/detail/DataTypes.h)

target_link_libraries(gelly_renderer PUBLIC d3d9.lib d3d11.lib d3dcompiler.lib)
target_include_directories(gelly_renderer PUBLIC include src)
target_include_directories(gelly_renderer PRIVATE src)

add_shader(NormalEstimationPS)
add_shader(NormalEstimationVS)
add_shader(ParticleRenderGS)
add_shader(ParticleRenderPS)
add_shader(ParticleRenderVS)
add_shader(ParticleSmoothingPS)
add_shader(ParticleSmoothingVS)
