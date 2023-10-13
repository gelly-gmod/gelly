add_subdirectory(vendor/SDL)

file(GLOB IMGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_sdl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_dx11.cpp
        )

add_executable(gelly_fluidsim_test
        main.cpp
        ${IMGUI_SOURCES}
        )

target_link_libraries(gelly_fluidsim_test
        PRIVATE
        SDL2
        gelly_fluid_sim)

target_include_directories(gelly_fluidsim_test
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
        )