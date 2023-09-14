include(PreprocessShader)
include(GetAllShaders)

find_hlsl_files("${SRC_DIR}/${LOCAL_SHADER_PATH}" D3D9_SHADER_NAMES)
find_hlsli_files("${SRC_DIR}/${LOCAL_SHADER_PATH}" D3D9_SHADER_INCLUDES)

preprocess_shaders("${D3D9_SHADER_NAMES}" "${D3D9_SHADER_INCLUDES}")