add_library(
        gelly_fluid_renderer
        STATIC
        include/GellyFluidRender.h
        src/GellyFluidRender.cpp
        include/fluidrender/IRenderContext.h include/fluidrender/IManagedTexture.h include/fluidrender/CD3D11RenderContext.h src/fluidrender/CD3D11RenderContext.cpp include/fluidrender/CD3D11ManagedTexture.h src/fluidrender/CD3D11ManagedTexture.cpp include/fluidrender/IManagedBuffer.h)

target_include_directories(
        gelly_fluid_renderer
        PUBLIC
        include
        ../gelly-interfaces/include
        ../gelly-d3d/include
)

target_link_libraries(
        gelly_fluid_renderer
        PUBLIC
        gelly_d3d9
)