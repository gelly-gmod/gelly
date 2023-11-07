include(GetAllShaders)
include(PreprocessShader)

find_hlsl_files("${SRC_DIR}/shaders" SHADER_FILES)
find_hlsli_files("${SRC_DIR}/shaders" SHADER_INCLUDE_FILES)

