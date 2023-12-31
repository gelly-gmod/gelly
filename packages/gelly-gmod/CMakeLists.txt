project(gelly-gmod CXX)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(vendor/gmod-module-base)
add_subdirectory(vendor/minhook)
add_subdirectory(vendor/GMFS)
add_subdirectory(vendor/BSPParser)

option(TRACY_ENABLE "Enable tracy profiler" ON)
option(TRACY_ON_DEMAND "Enable tracy profiler on demand" ON)
add_subdirectory(vendor/tracy)

add_subdirectory(../gelly "${CMAKE_CURRENT_BINARY_DIR}/gelly")

file(GLOB_RECURSE SHADER_GLUE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/out/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/out/*.h
        )

add_library(
    gelly-gmod SHARED
    src/main.cpp
    src/Gelly.cpp
    src/Gelly.h
    src/LoggingMacros.h
    src/source/Interface.h
    src/source/IBaseClientDLL.h
    src/source/MathTypes.h
    src/source/CViewSetup.h
    src/source/IBaseClientDLL.cpp
        src/source/IVRenderView.h
        src/source/IVRenderView.cpp
        src/source/D3DDeviceWrapper.h
        src/source/D3DDeviceWrapper.cpp
        src/hooking/Library.h
        src/hooking/Library.cpp
        ${SHADER_GLUE_FILES}
        src/source/CBaseEntity.h
        src/source/CBaseEntity.cpp
        src/source/IServerGameEnts.h
        src/source/IVEngineServer.h
        src/source/IVEngineServer.cpp
        src/source/IServerGameEnts.cpp
        src/source/Signatures.h
        src/source/GetCubemap.h
        src/source/GetCubemap.cpp
)

target_link_libraries(gelly-gmod
    PUBLIC
        ${GELLY_LIBS}
        gmod-module-base
        minhook
        GMFS
        BSPParser
        Tracy::TracyClient
)

target_include_directories(gelly-gmod
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/gmod-module-base/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/minhook/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/out
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/GMFS
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/BSPParser
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/tracy
        ${GELLY_INCLUDE_DIRS}
)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    set_target_properties(gelly-gmod PROPERTIES PREFIX "gmcl_")
    set_target_properties(gelly-gmod PROPERTIES SUFFIX "_win32.dll")
else()
    set_target_properties(gelly-gmod PROPERTIES PREFIX "gmcl_")
    set_target_properties(gelly-gmod PROPERTIES SUFFIX "_win64.dll")
endif()

add_custom_target(
        GellyGModShaders
        COMMAND ${CMAKE_COMMAND} -DSHADERS_DIR=${CMAKE_CURRENT_SOURCE_DIR}/src/shaders -DGSC_PATH=${GSC_PATH} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompileShaders.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Compiling gelly-gmod shaders"
)

add_dependencies(gelly-gmod GellyGModShaders)