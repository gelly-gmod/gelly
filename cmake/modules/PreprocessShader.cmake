# Builds a set where the key is a include directive and the value is the include-shader's normal HLSL content.
# This is used for preprocessing the include directive.
function(build_include_sets includeNames)
    foreach (include_name ${includeNames})
        set(include_path ${SRC_DIR}/${LOCAL_SHADER_PATH}/${include_name}.hlsli)
        file(READ ${include_path} include_code)
        # neat trick, we set the key as '#include <name>' to simplify finding the include directive
        list(APPEND include_set_directive "#include \"${include_name}.hlsli\"")
        # ... cmake hack
        string(REPLACE ";" "CMAKE_IS_STUPID" include_code "${include_code}")
        list(APPEND include_set_code ${include_code})
    endforeach ()
    set(include_set_directive ${include_set_directive} PARENT_SCOPE)
    set(include_set_code ${include_set_code} PARENT_SCOPE)
endfunction()

function(preprocess_include_directive shader_content)
    # Fed back into the foreach when multiple include directives are found
    set(output_content ${shader_content})
    foreach (directive code IN ZIP_LISTS include_set_directive include_set_code)
        message(STATUS "Replacing ${directive} with ${code}")
        # Undo cmake hack
        string(REPLACE "CMAKE_IS_STUPID" ";" code "${code}")
        string(REPLACE "${directive}" "${code}" output_content "${output_content}")
    endforeach ()
    set(output_content ${output_content} PARENT_SCOPE)
endfunction()

function(preprocess_shader name)
    message(STATUS "Adding shader ${shader_name} ")
    set(shader_path ${SRC_DIR}/${LOCAL_SHADER_PATH}/${shader_name}.hlsl)
    set(output_path ${SRC_DIR}/${LOCAL_SHADER_PATH}/${shader_name}.embed.hlsl)

    # Prepares the shader for embedding by adding R" === () === " around the shader code
    file(READ ${shader_path} shader_code)
    preprocess_include_directive("${shader_code}")
    set(shader_code ${output_content})
    set(shader_code "R\"===(${shader_code})===\"")
    file(WRITE ${output_path} "${shader_code}")
endfunction()

function(preprocess_shaders shaderNames includeNames)
    build_include_sets("${includeNames}")
    foreach (shader_name ${shaderNames})
        preprocess_shader(${shader_name})
    endforeach ()
endfunction()