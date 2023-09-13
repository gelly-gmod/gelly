include(PreprocessShader)

set(D3D11_SHADER_NAMES
        NDCQuad.vs
        ParticleRender.gs
        ParticleRender.ps
        ParticleRender.vs
        )

preprocess_shaders("${D3D11_SHADER_NAMES}")

# TODO: UPDATE ANYTHING USING SHADERS TO THIS NEW SYSTEM