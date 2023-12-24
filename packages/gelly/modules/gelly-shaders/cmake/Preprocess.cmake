include(PreprocessShader)
include(GetAllShaders)

find_hlsl_files("${SRC_DIR}/${LOCAL_SHADER_PATH}" SHADER_NAMES)
find_hlsli_files("${SRC_DIR}/${LOCAL_SHADER_PATH}" SHADER_INCLUDES)

preprocess_shaders("${SHADER_NAMES}" "${SHADER_INCLUDES}")