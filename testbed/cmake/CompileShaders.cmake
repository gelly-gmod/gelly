set(CMAKE_MODULE_PATH ${GELLY_MODULES_PATH})

include(GetAllShaders)
include(PreprocessShader)

find_hlsl_files("${SHADERS_DIR}" SHADER_FILES)

function(compute_target_profile path)
    # Our convention is simple: shader.ps/vsXX.hlsl
    # And we're only supporting SM 5.0, so its really just
    # shader.ps/vs50.hlsl

    get_filename_component(filename ${path} NAME)
    if (filename MATCHES ".*vs.*")
        set(TARGET_PROFILE "vs_5_0" PARENT_SCOPE)
    elseif (filename MATCHES ".*ps.*")
        set(TARGET_PROFILE "ps_5_0" PARENT_SCOPE)
    else ()
        message(FATAL_ERROR "Unknown shader type: ${path}")
    endif ()
endfunction()

foreach (SHADER_FILE ${SHADER_FILES})
    message(STATUS "Preprocessing ${SHADER_FILE}")
    set(OUTPUT_FILE "${SHADER_FILE}.dxbc")
    compute_target_profile(${SHADER_FILE})
    execute_process(
            COMMAND "fxc.exe" /T ${TARGET_PROFILE} /E main /Fo ${OUTPUT_FILE} ${SHADER_FILE}
            WORKING_DIRECTORY ${SHADERS_DIR}
    )
endforeach ()