# Since Testbed is programmed in tandem with Gelly, we do skip
# the part where we would add the gelly subdirectory.

set(CMAKE_CXX_STANDARD 17)

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
        ${IMGUI_SOURCES} src/Shaders.h src/Shaders.cpp src/Scene.cpp src/Scene.h src/Camera.cpp src/Camera.h
        src/D3D11DebugLayer.cpp
        src/D3D11DebugLayer.h
)

add_subdirectory(vendor/SDL)
add_subdirectory(vendor/fastgltf)

generate_gelly_config()

target_link_libraries(
        testbed
        SDL2
        fastgltf
        ${GELLY_LIBS}
)

target_include_directories(
        testbed
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/fastgltf/include
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

# Compile shaders by invoking the script in cmake/CompileShaders.cmake
add_custom_target(
        TestbedShaders
        COMMAND ${CMAKE_COMMAND} -DSHADERS_DIR=${CMAKE_CURRENT_SOURCE_DIR}/src/shaders -DGELLY_MODULES_PATH=${CMAKE_MODULE_PATH} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompileShaders.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders
        COMMENT "Compiling shaders"
)

# And then the copy target
add_custom_target(
        TestbedAssetCopy
        DEPENDS TestbedShaders
        COMMAND ${CMAKE_COMMAND} -DROOT_DIR=${CMAKE_CURRENT_SOURCE_DIR} -DOUTPUT_DIR=${CMAKE_BINARY_DIR}/testbed -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CopyAssets.cmake
        COMMENT "Copying assets"
)

add_dependencies(testbed TestbedAssetCopy)
add_dependencies(testbed TestbedShaders)