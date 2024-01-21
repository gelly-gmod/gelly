option(GELLY_ENABLE_CPU_VISUALIZER "Enable CPU Algorithm Visualizer" ON)

# Since our library is header only, we can just add it as a target
add_library(gelly_cpu_refs INTERFACE
        lib/include/gelly-cpu-refs/Logging.h
        lib/include/gelly-cpu-refs/algo/marching-cubes.h
        lib/include/gelly-cpu-refs/algo/marching-cubes-lut.h
        lib/include/gelly-cpu-refs/structs/HashTable.h
        lib/include/gelly-cpu-refs/debugging/IVisualDebugFacility.h
)

add_subdirectory(vendor/DirectXMath)

target_link_libraries(gelly_cpu_refs
        INTERFACE
        DirectXMath
)

target_include_directories(gelly_cpu_refs
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/DirectXMath/Inc
)

if (GELLY_ENABLE_CPU_VISUALIZER)
    message(STATUS "Gelly CPU algorithm visualizer enabled")
    add_executable(gelly_cpu_visualizer
            src/Logging.h
            src/main.cpp
            src/WindowConstants.h
            src/IVisualizer.h
            src/visualizers/Example.h
            src/visualizers/Example.cpp
            src/CRaylibVisualDebugFacility.h
            src/CRaylibVisualDebugFacility.cpp
            src/visualizers/CMarchingCubesVisualizer.cpp
            src/visualizers/CMarchingCubesVisualizer.h
    )

    add_subdirectory(vendor/raylib)
    target_link_libraries(gelly_cpu_visualizer
            PRIVATE
            raylib
            gelly_cpu_refs
    )

    target_include_directories(gelly_cpu_visualizer
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/raylib/src
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
    )

    set_target_properties(gelly_cpu_visualizer PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS NO
    )
endif ()