add_library(
        gelly_fluid_renderer
        STATIC
        include/GellyFluidRender.h
        src/GellyFluidRender.cpp
)

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