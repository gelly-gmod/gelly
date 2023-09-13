option(BUILD_GELLY_RENDERER_TEST "Build Gelly Renderer Test" OFF)

if (BUILD_GELLY_RENDERER_TEST)
    message(STATUS "Copying shaders to test directory")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test)
    add_subdirectory(test)
endif ()

add_library(gelly_renderer STATIC
        include/GellyRenderer.h
        src/GellyRenderer.cpp include/detail/ErrorHandling.h
        src/Camera.cpp
        src/rendering/Technique.h
        src/rendering/techniques/ParticleRendering.cpp
        src/rendering/techniques/ParticleRendering.h include/detail/DataTypes.h src/rendering/techniques/PerFrameCBuffer.h src/rendering/SSTechnique.cpp include/detail/GBuffer.h)

target_link_libraries(gelly_renderer PUBLIC gelly_d3d9 d3d9.lib d3d11.lib d3dcompiler.lib)
target_include_directories(gelly_renderer PRIVATE src include ../gelly-d3d9/include ../gelly-shaders)
target_include_directories(gelly_renderer PUBLIC src include ../gelly-d3d9/include ../gelly-shaders)

add_dependencies(gelly_renderer GellyD3D11Shaders)