option(BUILD_GELLY_RENDERER_TEST "Build Gelly Renderer Test" OFF)

if (BUILD_GELLY_RENDERER_TEST)
    add_subdirectory(test)
endif ()

add_library(gelly_renderer STATIC
        include/GellyRenderer.h
        src/GellyRenderer.cpp include/detail/ErrorHandling.h
        src/Camera.cpp
        src/rendering/Technique.h
        src/rendering/techniques/ParticleRendering.cpp
        src/rendering/techniques/ParticleRendering.h src/rendering/techniques/PerFrameCBuffer.h src/rendering/SSTechnique.cpp include/detail/GBuffer.h src/rendering/techniques/OutputEncoder.cpp src/rendering/techniques/OutputEncoder.h src/rendering/techniques/DepthSmoothing.cpp src/rendering/techniques/DepthSmoothing.h src/rendering/techniques/NormalEstimation.cpp src/rendering/techniques/NormalEstimation.h src/rendering/techniques/IsosurfaceExtraction.cpp src/rendering/techniques/IsosurfaceExtraction.h)

target_link_libraries(gelly_renderer PUBLIC gelly_d3d9 d3d9.lib d3d11.lib d3dcompiler.lib)
target_include_directories(gelly_renderer PRIVATE src include ../gelly-d3d9/include ../gelly-shaders)
target_include_directories(gelly_renderer PUBLIC src include ../gelly-d3d9/include ../gelly-shaders)

add_dependencies(gelly_renderer GellyD3D11Shaders)