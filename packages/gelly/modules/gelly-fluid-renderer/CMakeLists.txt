# We automatically compile any auto-generated shader glue code
file(GLOB_RECURSE SHADER_GLUE_CODE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/out/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/out/*.h"
)

add_library(
        gelly_fluid_renderer
        STATIC
        ${SHADER_GLUE_CODE}
        include/GellyFluidRender.h
        src/GellyFluidRender.cpp
        include/fluidrender/IRenderContext.h include/fluidrender/IManagedTexture.h include/fluidrender/CD3D11RenderContext.h src/fluidrender/CD3D11RenderContext.cpp include/fluidrender/CD3D11ManagedTexture.h src/fluidrender/CD3D11ManagedTexture.cpp include/fluidrender/IManagedBuffer.h include/fluidrender/CD3D11ManagedBuffer.h src/fluidrender/CD3D11ManagedBuffer.cpp include/fluidrender/IFluidRenderer.h src/fluidrender/CD3D11to11SharedTexture.cpp include/fluidrender/CD3D11to11SharedTexture.h include/fluidrender/IFluidTextures.h include/fluidrender/CD3D11DebugFluidTextures.h src/fluidrender/CD3D11DebugFluidTextures.cpp include/fluidrender/CD3D11DebugFluidRenderer.h src/fluidrender/CD3D11DebugFluidRenderer.cpp
        include/fluidrender/IManagedShader.h
        src/fluidrender/CD3D11ManagedShader.cpp
        include/fluidrender/CD3D11ManagedShader.h
        include/fluidrender/IManagedBufferLayout.h
        include/fluidrender/CD3D11ManagedBufferLayout.h
        src/fluidrender/CD3D11ManagedBufferLayout.cpp
        include/fluidrender/util/CBuffers.h
        include/fluidrender/IManagedDepthBuffer.h
        include/fluidrender/CD3D11ManagedDepthBuffer.h
        src/fluidrender/CD3D11ManagedDepthBuffer.cpp
        include/fluidrender/util/ScreenQuadVB.h)

target_include_directories(
        gelly_fluid_renderer
        PUBLIC
        include
        src/shaders/out
        ../gelly-interfaces/include
        ../gelly-d3d/include
        ../gelly-fluid-sim/include
        # For the in-application API
        vendor/renderdoc/renderdoc/api/app
)

target_link_libraries(
        gelly_fluid_renderer
        PUBLIC
        gelly_d3d9
        gelly_fluid_sim
)

add_custom_target(
        GellyShaderCompile
        COMMAND ${CMAKE_COMMAND} -DSHADERS_DIR=${CMAKE_CURRENT_SOURCE_DIR}/src/shaders -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH} -DGSC_PATH=${GSC_PATH} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompileShaders.cmake
        COMMENT "Compiling all Gelly shaders"
)

add_dependencies(
        gelly_fluid_renderer
        gsc
        GellyShaderCompile
)