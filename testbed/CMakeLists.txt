# Since Testbed is programmed in tandem with Gelly, we do skip
# the part where we would add the gelly subdirectory.

set(IMGUI_SOURCES
        vendor/imgui/imgui.cpp
        vendor/imgui/imgui_demo.cpp
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_widgets.cpp
        vendor/imgui/imgui_tables.cpp
        vendor/imgui/backends/imgui_impl_sdl2.cpp
        vendor/imgui/backends/imgui_impl_dx11.cpp
        )

add_executable(testbed
        src/main.cpp
        src/Window.h src/Window.cpp src/Logging.h src/ILogger.h src/CConsoleLogger.cpp src/CConsoleLogger.h src/Logging.cpp src/Rendering.h src/Rendering.cpp
        ${IMGUI_SOURCES})

add_subdirectory(vendor/SDL)

generate_gelly_config()

target_link_libraries(
        testbed
        SDL2
        ${GELLY_LIBS}
)

target_include_directories(
        testbed
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
        ${GELLY_INCLUDE_DIRS}
)

# https://github.com/libsdl-org/SDL/issues/6399#issuecomment-1280612287
if (WIN32)
    add_custom_command(
            TARGET testbed POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>" "$<TARGET_FILE_DIR:testbed>"
            VERBATIM
    )
endif ()