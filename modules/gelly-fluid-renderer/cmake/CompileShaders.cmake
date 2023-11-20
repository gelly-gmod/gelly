include(GetAllShaders)

find_hlsl_files(${SHADERS_DIR} SHADER_FILES)

foreach (SHADER_FILE ${SHADER_FILES})
    execute_process(
            COMMAND ${GSC_PATH} ${SHADER_FILE}
            WORKING_DIRECTORY ${SHADERS_DIR}
            COMMAND_ERROR_IS_FATAL ANY
    )
endforeach ()