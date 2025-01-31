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

# compute the GBP binaries we'll need to compile (basically just each 'src/binaries/gbp/*.h' and each 'src/binaries/gbp/*.cpp') file
set(GBP_SOURCE_FILES "")
# then figure out the dependencies
foreach (FLEX_DEP ${FLEX_LIBS})
    get_filename_component(FLEX_DEP_NAME ${FLEX_DEP} NAME_WE) # very important that we select WE, some files have multiple extensions
    list(APPEND GBP_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/binaries/gbp/${FLEX_DEP_NAME}.cpp)
    list(APPEND GBP_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/binaries/gbp/${FLEX_DEP_NAME}.h)
endforeach ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # force -O0
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od")
    message(WARNING "Debug build detected, forcing -O0")
endif ()

message(STATUS "GBP_SOURCE_FILES: ${GBP_SOURCE_FILES}")

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
        ${GBP_SOURCE_FILES}
        src/binaries/gbp.h
        src/composite/reload-shaders.h
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
        delayimp.lib
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
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/PHYParser
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/MemoryModule
        ${GELLY_INCLUDE_DIRS}
)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL " x86_64")
    set_target_properties(gelly-gmod PROPERTIES PREFIX "gmcl_")
    set_target_properties(gelly-gmod PROPERTIES SUFFIX "_win32.dll")
else ()
    set_target_properties(gelly-gmod PROPERTIES PREFIX "gmcl_")
    set_target_properties(gelly-gmod PROPERTIES SUFFIX "_win64.dll ")
endif ()

# Must delay load the FleX DLLs, otherwise we can't extract them from the pack in time.
# Don't know if other compilers support, so we'll leave a warning here.
if (NOT MSVC)
    message(WARNING "Delay loading FleX DLLs is only known to work correctly on MSVC.\
    If you're using another compiler, you may need to find an alternative solution.\
    Feel free to PR a fix if you find one!\
    The important part is that your built Gelly will likely malfunction on first load if you don't find a way to enable delay loading for your linker.\
    Since we leave behind the packaged binaries, a second load will work fine. Just exercise caution, clang-cl may work, but it is untested.
    Please don't distribute a build that doesn't work on first load.")
endif ()

set(CMAKE_SHARED_LINKER_FLAGS " ${CMAKE_SHARED_LINKER_FLAGS} /DELAYLOAD:NVFLEXRELEASED3D_X64.DLL /DELAYLOAD:NVFLEXEXTRELEASED3D_X64.DLL")

# add our version header
message(STATUS "Configuring gelly-gmod's version as ${GELLY_VERSION}")
configure_file(src/version.h.in src/version.h @ONLY)
target_include_directories(gelly-gmod PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

set_target_properties(gelly-gmod PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${GELLY_ROOT_DIR}/release/garrysmod/lua/bin
)

# gbp operates within the scope of a pack--so we don't have to individually create commands for each flex dep--we can just make 1 target to call gbp
set(GBP_DEPS "")

foreach (FLEX_DEP ${FLEX_LIBS})
    string(APPEND GBP_DEPS "${FLEX_DEP},")
endforeach ()

# remove the trailing comma
string(LENGTH ${GBP_DEPS} GBP_DEPS_LENGTH)
math(EXPR GBP_DEPS_LENGTH "${GBP_DEPS_LENGTH} - 1")
string(SUBSTRING ${GBP_DEPS} 0 ${GBP_DEPS_LENGTH} GBP_DEPS)

message(STATUS "GBP_DEPS: ${GBP_DEPS}")

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/binaries/gbp.h
        BYPRODUCTS ${GBP_SOURCE_FILES}
        COMMAND ${GBP_PATH} ${GBP_DEPS} ${CMAKE_CURRENT_SOURCE_DIR}/src/binaries
        DEPENDS ${GBP_PATH}
        COMMENT "Creating the GBP pack of FleX dependencies"
)

add_custom_target(
        CreateGBPPack
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/binaries/gbp.h
        COMMENT "Creating the GBP pack of FleX dependencies"
)

# we also want to version the binary after it's built
if (RCEDIT)
    message(STATUS "Versioning gelly-gmod binary as: ${GELLY_VERSION}")
    add_custom_command(
            POST_BUILD
            COMMAND ${RCEDIT} \"$<TARGET_FILE:gelly-gmod>\" --set-file-version \"${GELLY_VERSION}\" --set-version-string \"companyname\" \"Gelly\" --set-version-string \"productname\" \"Gelly binary module\" --set-version-string \"filedescription\" \"Gelly's binary module for Garry's Mod\"
            TARGET gelly-gmod
    )
endif ()

add_dependencies(gelly-gmod gsc gbp GellyGModShaderCompile CreateGBPPack)