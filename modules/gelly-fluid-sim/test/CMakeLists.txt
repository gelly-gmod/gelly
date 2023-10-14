add_subdirectory(vendor/SDL)
add_subdirectory(vendor/DirectXMath)

file(GLOB IMGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_sdl2.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_dx11.cpp
        )

add_executable(gelly_fluidsim_test
        src/main.cpp
        ${IMGUI_SOURCES}
        )

target_link_libraries(gelly_fluidsim_test
        PUBLIC
        SDL2
        gelly_fluid_sim
        d3d11
        gelly_d3d9)

target_include_directories(gelly_fluidsim_test
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/DirectXMath/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../gelly-shaders/
        )

# We have to manually copy the SDL2.dll to the build directory
add_custom_command(
        TARGET gelly_fluidsim_test POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>" "$<TARGET_FILE_DIR:gelly_fluidsim_test>"
        VERBATIM
)

# We also have to explicitly mark the shaders as a build dependency
add_dependencies(gelly_fluidsim_test GellyD3D11Shaders)