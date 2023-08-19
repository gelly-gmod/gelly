add_subdirectory(vendor/gmod-module-base)
add_subdirectory(vendor/minhook)
add_subdirectory(vendor/GMFS)

add_library(gelly_binary_module SHARED
        src/main.cpp src/TextureOverride.h src/TextureOverride.cpp src/Gelly.h src/Gelly.cpp)
target_include_directories(gelly_binary_module PRIVATE
        vendor/gmod-module-base/include
        vendor/minhook/include
        vendor/GMFS
        ../gelly-engine/include
        ../gelly-renderer/include
        )
target_link_libraries(gelly_binary_module PRIVATE minhook gelly_engine gelly_renderer GMFS d3d9.lib)
# The dlls are named gmcl_<module_name>_<arch>.dll
if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
    set_target_properties(gelly_binary_module PROPERTIES OUTPUT_NAME gmcl_gelly_win64)
elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86")
    set_target_properties(gelly_binary_module PROPERTIES OUTPUT_NAME gmcl_gelly_win32)
endif ()