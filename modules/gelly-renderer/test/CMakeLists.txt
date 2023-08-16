add_subdirectory(vendor/SDL)
add_executable(gelly_renderer_test
        src/main.cpp
        src/BasicD3D9Renderer.h
        src/BasicD3D9Renderer.cpp
        src/ErrorHandling.h)
target_include_directories(gelly_renderer_test PRIVATE vendor/SDL/include)
target_link_libraries(gelly_renderer_test gelly_renderer d3d9.lib SDL2)

if (WIN32)
    add_custom_command(
            TARGET gelly_renderer_test POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>" "$<TARGET_FILE_DIR:gelly_renderer_test>"
            VERBATIM
    )
endif()