include(GetFleXArchitecture)
option(BUILD_GELLY_ENGINE_TEST "Build the gelly-engine test executable" OFF)

GetFleXArchitecture()

set(FLEX_LIBRARIES
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/FleX/lib/${ARCH}/NvFlexReleaseD3D_${ARCH_SUFFIX}.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/FleX/lib/${ARCH}/NvFlexDeviceRelease_${ARCH_SUFFIX}.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/FleX/lib/${ARCH}/NvFlexExtReleaseD3D_${ARCH_SUFFIX}.lib"
        )

add_subdirectory(vendor/BSPParser)

add_library(gelly_engine STATIC
        src/GellyEngine.cpp
        include/GellyEngine.h
        src/GellyScene.cpp
        include/GellyScene.h
        src/Colliders.cpp
        src/MeshConvert.h src/MeshConvert.cpp)

target_link_libraries(gelly_engine
        ${FLEX_LIBRARIES} BSPParser)

target_include_directories(gelly_engine PUBLIC
        include)

target_include_directories(gelly_engine PUBLIC
        vendor/FleX/include
        vendor/BSPParser)

if (BUILD_GELLY_ENGINE_TEST)
    add_subdirectory(vendor/raylib)
    add_subdirectory(vendor/glm)
    add_executable(gelly_engine_test test/main.cpp)
    target_link_libraries(gelly_engine_test gelly_engine raylib)
    target_include_directories(gelly_engine_test PRIVATE vendor/raylib/src vendor/raygui/src vendor/glm/)
endif ()