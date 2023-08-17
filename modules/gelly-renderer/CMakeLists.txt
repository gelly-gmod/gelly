option(BUILD_GELLY_RENDERER_TEST "Build Gelly Renderer Test" OFF)

if (BUILD_GELLY_RENDERER_TEST)
    message(STATUS "Copying shaders to test directory")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test)
    add_subdirectory(test)
endif()

add_library(gelly_renderer STATIC
        include/GellyRenderer.h
        src/GellyRenderer.cpp include/detail/ErrorHandling.h
        include/detail/Shader.h src/Shader.cpp
        src/Camera.cpp)

target_link_libraries(gelly_renderer PUBLIC d3d9.lib d3d11.lib d3dcompiler.lib)
target_include_directories(gelly_renderer PUBLIC include src)
target_include_directories(gelly_renderer PRIVATE src)
