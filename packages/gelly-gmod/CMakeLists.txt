project(gelly-gmod CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${GELLY_ROOT_DIR}/cmake")

include(CreateShaderGlueTarget)

add_subdirectory(vendor/gmod-module-base)
add_subdirectory(vendor/minhook)
add_subdirectory(vendor/GMFS)
add_subdirectory(vendor/BSPParser)
set(BUILD_PHYPARSER_TEST OFF)
add_subdirectory(vendor/PHYParser)

option(GELLY_ENABLE_RENDERDOC_CAPTURES "Enable RenderDoc capture functionality" OFF)

set(TRACY_ENABLE OFF CACHE BOOL " Enable tracy" FORCE)
set(TRACY_ON_DEMAND OFF CACHE BOOL "Enable tracy on demand" FORCE)
# Incredibly important as we use tracy in a dll
set(TRACY_MANUAL_LIFETIME ON CACHE BOOL "Don't use tracy's lifetime feature" FORCE)
set(TRACY_DELAYED_INIT ON CACHE BOOL "Don't use tracy's delayed init feature" FORCE)

add_subdirectory(vendor/tracy)
add_subdirectory(../gelly "gelly")

create_shader_glue_targets(GellyGModShaderCompile "Compiling the GMod module's shaders" ${GELLY_PRODUCTION_BUILD})

add_library(
        gelly-gmod SHARED
        ${SHADER_OUTPUT_FILES}
        src/main.cpp
        src/logging/global-macros.h
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
        src/logging/format-log-entry.h
        src/logging/log-entry.h
        src/logging/severity-strings.h
        src/logging/format-log-entry.cpp
        src/logging/log.cpp
        src/logging/log.h
        src/logging/helpers/save-log-to-file.cpp
        src/logging/helpers/save-log-to-file.h
        src/logging/log-events.cpp
        src/logging/log-events.h
        src/logging/global-macros.cpp
        src/logging/helpers/dev-console-logging.cpp
        src/logging/helpers/dev-console-logging.h
        src/exceptions/generate-stack-trace.cpp
        src/exceptions/generate-stack-trace.h
        src/luajit/raw-lua-access.cpp
        src/luajit/raw-lua-access.h
        src/luajit/setup-atpanic-handler.cpp
        src/luajit/setup-atpanic-handler.h
        src/exceptions/get-stack-size.cpp
        src/exceptions/get-stack-size.h
        src/version.h.in
        src/util/lua-table.h
        src/util/PHYToGMod.h
        src/scene/asset-cache.cpp
        src/scene/asset-cache.h
        src/util/parse-asset-from-filesystem.h
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
        PHYParser
        Tracy::TracyClient
        DbgHelp.lib
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
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/PHYParser/src
        ${GELLY_INCLUDE_DIRS}
)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL " x86_64")
    set_target_properties(gelly-gmod PROPERTIES PREFIX "gmcl_")
    set_target_properties(gelly-gmod PROPERTIES SUFFIX "_win32.dll")
else ()
    set_target_properties(gelly-gmod PROPERTIES PREFIX "gmcl_")
    set_target_properties(gelly-gmod PROPERTIES SUFFIX "_win64.dll ")
endif ()

# add our version header
message(STATUS "Configuring gelly-gmod's version as ${GELLY_VERSION}")
configure_file(src/version.h.in src/version.h @ONLY)
target_include_directories(gelly-gmod PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

set_target_properties(gelly-gmod PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${GELLY_ROOT_DIR}/release/garrysmod/lua/bin
)

foreach (FLEX_DEP ${FLEX_LIBS})
    add_custom_command(TARGET gelly-gmod POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${FLEX_DEP}
            ${GELLY_ROOT_DIR}/release
            VERBATIM
            COMMENT "Copying ${FLEX_DEP} to the output directory"
    )
endforeach ()

add_custom_target(
        CopyFlexLibs
        DEPENDS ${FLEX_LIBS}
)

add_dependencies(gelly-gmod gsc GellyGModShaderCompile CopyFlexLibs)
