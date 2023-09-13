function(preprocess_shader name)
    message(STATUS "Adding shader ${shader_name}")
    set(shader_path ${SRC_DIR}/${LOCAL_SHADER_PATH}/${shader_name}.hlsl)
    set(output_path ${SRC_DIR}/${LOCAL_SHADER_PATH}/${shader_name}.embed.hlsl)

    # Prepares the shader for embedding by adding R"===()===" around the shader code
    file(READ ${shader_path} shader_code)
    set(shader_code "R\"===(${shader_code})===\"")
    file(WRITE ${output_path} "${shader_code}")
endfunction()

function(preprocess_shaders shaderNames)
    foreach (shader_name ${shaderNames})
        preprocess_shader(${shader_name})
    endforeach ()
endfunction()