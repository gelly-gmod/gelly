project(gelly-gmod CXX)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(vendor/gmod-module-base)

add_library(
    gelly-gmod SHARED
    src/main.cpp
)

target_include_directories(gelly-gmod
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/gmod-module-base/include
)

# add gmcl_ and architecture suffixes

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    set_target_properties(gelly-gmod PROPERTIES PREFIX "gmcl_")
    set_target_properties(gelly-gmod PROPERTIES SUFFIX "_win32.dll")
else()
    set_target_properties(gelly-gmod PROPERTIES PREFIX "gmcl_")
    set_target_properties(gelly-gmod PROPERTIES SUFFIX "_win64.dll")
endif()