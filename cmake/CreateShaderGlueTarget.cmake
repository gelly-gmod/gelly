include(GetAllShaders)

function(create_shader_glue_targets name comment)
    find_hlsl_files(${CMAKE_CURRENT_SOURCE_DIR}/src/shaders SHADER_FILES)
    # keep track of our output files so we can add them as dependencies
    set(SHADER_OUTPUT_FILES "")

    foreach (SHADER_FILE ${SHADER_FILES})
        get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WLE)
        # also grab the relative parent directory for nested shaders
        get_filename_component(SHADER_PARENT_DIR ${SHADER_FILE} DIRECTORY)

        # translate from raw name to gsc name (Foam.psXX -> FoamPS) where XX is the shader version
        string(REGEX REPLACE "([a-zA-Z]+)\\.ps([0-9]+)" "\\1PS" SHADER_NAME ${SHADER_NAME})
        string(REGEX REPLACE "([a-zA-Z]+)\\.vs([0-9]+)" "\\1VS" SHADER_NAME ${SHADER_NAME})
        string(REGEX REPLACE "([a-zA-Z]+)\\.cs([0-9]+)" "\\1CS" SHADER_NAME ${SHADER_NAME})
        string(REGEX REPLACE "([a-zA-Z]+)\\.gs([0-9]+)" "\\1GS" SHADER_NAME ${SHADER_NAME})
        add_custom_command(
                OUTPUT
                ${SHADER_PARENT_DIR}/out/${SHADER_NAME}.cpp
                ${SHADER_PARENT_DIR}/out/${SHADER_NAME}.h
                COMMAND ${GSC_PATH} ${SHADER_FILE}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders # critical so that our glue code is generated in the correct directory
                DEPENDS ${SHADER_FILE}
                COMMENT "Compiling shader ${SHADER_NAME}.cpp and ${SHADER_NAME}.h"
        )

        list(APPEND SHADER_OUTPUT_FILES ${SHADER_PARENT_DIR}/out/${SHADER_NAME}.cpp)
        list(APPEND SHADER_OUTPUT_FILES ${SHADER_PARENT_DIR}/out/${SHADER_NAME}.h)
    endforeach ()

    add_custom_target(
            ${name}
            DEPENDS ${SHADER_OUTPUT_FILES}
            SOURCES ${SHADER_FILES}
            COMMENT ${comment}
    )

    # propagate the output files to the parent scope
    set(SHADER_OUTPUT_FILES ${SHADER_OUTPUT_FILES} PARENT_SCOPE)
endfunction()
