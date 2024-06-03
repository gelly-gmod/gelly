# We automatically compile any auto-generated shader glue code

include(CreateShaderGlueTarget)

create_shader_glue_targets(GellyShaderCompile "Compiling the fluid renderer's shaders" , ${GELLY_PRODUCTION_BUILD})
add_library(
        gelly_fluid_renderer
        STATIC
        ${SHADER_OUTPUT_FILES}
        include/GellyFluidRender.h
        src/GellyFluidRender.cpp
        include/fluidrender/IRenderContext.h include/fluidrender/IManagedTexture.h include/fluidrender/CD3D11RenderContext.h src/fluidrender/CD3D11RenderContext.cpp include/fluidrender/CD3D11ManagedTexture.h src/fluidrender/CD3D11ManagedTexture.cpp include/fluidrender/IManagedBuffer.h include/fluidrender/CD3D11ManagedBuffer.h src/fluidrender/CD3D11ManagedBuffer.cpp include/fluidrender/IFluidRenderer.h src/fluidrender/CD3D11to11SharedTexture.cpp include/fluidrender/CD3D11to11SharedTexture.h include/fluidrender/IFluidTextures.h include/fluidrender/splatting/CD3D11SplattingFluidTextures.h src/fluidrender/CD3D11DebugFluidTextures.cpp include/fluidrender/splatting/CD3D11SplattingFluidRenderer.h src/fluidrender/CD3D11DebugFluidRenderer.cpp
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
        include/fluidrender/util/ScreenQuadVB.h
        include/fluidrender/CD3D9to11SharedTexture.h
        src/fluidrender/CD3D9to11SharedTexture.cpp
        include/fluidrender/IMappedBufferView.h
        include/fluidrender/CD3D11MappedBufferView.h
        src/fluidrender/CD3D11MappedBufferView.cpp
        include/fluidrender/util/Kernel.h
        src/fluidrender/util/Kernel.cpp
        include/fluidrender/isosurface/CD3D11IsosurfaceFluidRenderer.h
        src/fluidrender/isosurface/CD3D11IsosurfaceFluidRenderer.cpp
        include/fluidrender/util/Exceptions.h
        include/fluidrender/IPerfMarker.h
        include/fluidrender/CD3D11PerfMarker.h
        src/fluidrender/CD3D11PerfMarker.cpp)

target_include_directories(
        gelly_fluid_renderer
        PUBLIC
        include
        src/shaders/out
        src/shaders/isosurface/out
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
        dxguid.lib
)

# If tracy is being used we need to link to it
if (TRACY_ENABLE)
    message(STATUS "Tracy is enabled, using in gelly-fluid-renderer")
    target_link_libraries(
            gelly_fluid_renderer
            PUBLIC
            TracyClient
    )
    # also include the tracy headers
    target_include_directories(
            gelly_fluid_renderer
            PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/tracy
    )
endif ()

# Propagate GELLY_ENABLE_RENDERDOC_CAPTURES down to compile definitions
if (GELLY_ENABLE_RENDERDOC_CAPTURES)
    add_compile_definitions("GELLY_ENABLE_RENDERDOC_CAPTURES")
endif ()

add_dependencies(
        gelly_fluid_renderer
        gsc
        GellyShaderCompile
)