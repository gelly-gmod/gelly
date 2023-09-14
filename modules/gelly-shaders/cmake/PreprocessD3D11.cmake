include(PreprocessShader)

set(D3D11_SHADER_NAMES
        NDCQuad.vs
        ParticleRender.gs
        ParticleRender.ps
        ParticleRender.vs
        )

set(D3D11_SHADER_INCLUDES
        ParticleRenderStages
        PerFrameCB
        )

preprocess_shaders("${D3D11_SHADER_NAMES}" "${D3D11_SHADER_INCLUDES}")