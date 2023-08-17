add_subdirectory(vendor/gmod-module-base)
add_library(gelly_binary_module SHARED
        src/main.cpp)
target_include_directories(gelly_binary_module PRIVATE vendor/gmod-module-base/include)

# The dlls are named gmcl_<module_name>_<arch>.dll
if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
    set_target_properties(gelly_binary_module PROPERTIES OUTPUT_NAME gmcl_gelly_win64)
elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86")
    set_target_properties(gelly_binary_module PROPERTIES OUTPUT_NAME gmcl_gelly_win32)
endif ()