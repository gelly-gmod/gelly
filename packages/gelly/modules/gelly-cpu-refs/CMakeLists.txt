option(GELLY_ENABLE_CPU_VISUALIZER "Enable CPU Algorithm Visualizer" ON)

if (GELLY_ENABLE_CPU_VISUALIZER)
    message(STATUS "Gelly CPU algorithm visualizer enabled")
    add_executable(gelly_cpu_visualizer
            src/Logging.h
            src/main.cpp
            src/WindowConstants.h
            src/IVisualizer.h
            src/visualizers/Example.h
            src/visualizers/Example.cpp
    )

    add_subdirectory(vendor/raylib)
    target_link_libraries(gelly_cpu_visualizer
            PRIVATE
            raylib
    )

    target_include_directories(gelly_cpu_visualizer
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/raylib/src
    )
endif ()