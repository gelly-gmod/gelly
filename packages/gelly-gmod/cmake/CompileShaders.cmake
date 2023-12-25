function(find_hlsl_files directory output_var)
    file(GLOB_RECURSE files "${directory}/*.hlsl")
    foreach (file ${files})
        if (${file} MATCHES ".+embed")
            continue()
        endif ()
        # Strip last character
        list(APPEND file_names ${file})
    endforeach ()
    set(${output_var} ${file_names} PARENT_SCOPE)
endfunction()

find_hlsl_files(${SHADERS_DIR} SHADER_FILES)

message(STATUS "Using shader compiler: ${GSC_PATH}")
foreach (SHADER_FILE ${SHADER_FILES})
    execute_process(
            COMMAND ${GSC_PATH} ${SHADER_FILE}
            WORKING_DIRECTORY ${SHADERS_DIR}
    )
endforeach ()