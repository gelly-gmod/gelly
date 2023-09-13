include(PreprocessShader)

set(D3D9_SHADER_NAMES
        Composite.ps
        Composite.vs
        )

preprocess_shaders("${D3D9_SHADER_NAMES}" "")