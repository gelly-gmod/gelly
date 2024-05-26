project(gelly-gmod CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${GELLY_ROOT_DIR}/cmake")

include(CreateShaderGlueTarget)

add_subdirectory(vendor/gmod-module-base)
add_subdirectory(vendor/minhook)
add_subdirectory(vendor/GMFS)
add_subdirectory(vendor/BSPParser)

option(GELLY_ENABLE_RENDERDOC_CAPTURES "Enable RenderDoc capture functionality" OFF)

set(TRACY_ENABLE OFF CACHE BOOL " Enable tracy" FORCE)
set(TRACY_ON_DEMAND OFF CACHE BOOL "Enable tracy on demand" FORCE)
# Incredibly important as we use tracy in a dll
set(TRACY_MANUAL_LIFETIME ON CACHE BOOL "Don't use tracy's lifetime feature" FORCE)
set(TRACY_DELAYED_INIT ON CACHE BOOL "Don't use tracy's delayed init feature" FORCE)
add_subdirectory(vendor/tracy)

add_subdirectory(../gelly "gelly")

create_shader_glue_targets(GellyGModShaderCompile "Compiling the GMod module's shaders")
add_library(
        gelly-gmod SHARED
        ${SHADER_OUTPUT_FILES}
        src/main.cpp
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
        src/source/CBaseEntity.h
        src/source/CBaseEntity.cpp
        src/source/IServerGameEnts.h
        src/source/IVEngineServer.h
        src/source/IVEngineServer.cpp
        src/source/IServerGameEnts.cpp
        src/source/Signatures.h
        src/source/GetCubemap.h
        src/source/GetCubemap.cpp
        src/scene/Scene.cpp
        src/scene/Scene.h
        src/scene/EntityManager.cpp
        src/scene/EntityManager.h
        src/scene/EntIndex.h
        src/scene/Map.cpp
        src/scene/Map.h
        src/scene/ParticleManager.cpp
        src/scene/ParticleManager.h
        src/scene/Config.cpp
        src/scene/Config.h
        src/composite/GModCompositor.cpp
        src/composite/GModCompositor.h
        src/composite/Pipeline.h
        src/composite/UnownedResources.h
        src/composite/standard/StandardPipeline.cpp
        src/composite/standard/StandardPipeline.h
        src/composite/standard/StandardTextures.cpp
        src/composite/standard/StandardTextures.h
        src/composite/GellyResources.h
        src/composite/PipelineConfig.h
        src/composite/Resources.cpp
        src/composite/Resources.h
        src/composite/PipelineType.h
        src/util/GellySharedPtrs.h
)

# Propagate GELLY_ENABLE_RENDERDOC_CAPTURES down to compile definitions
if (GELLY_ENABLE_RENDERDOC_CAPTURES)
    add_compile_definitions("GELLY_ENABLE_RENDERDOC_CAPTURES")
endif ()

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

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL " x86_64")
    set_target_properties(gelly-gmod PROPERTIES PREFIX "gmcl_")
    set_target_properties(gelly-gmod PROPERTIES SUFFIX "_win32.dll")
else ()
    set_target_properties(gelly-gmod PROPERTIES PREFIX "gmcl_")
    set_target_properties(gelly-gmod PROPERTIES SUFFIX "_win64.dll ")
endif ()

add_dependencies(gelly-gmod gsc GellyGModShaderCompile)