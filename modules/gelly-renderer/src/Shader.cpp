#include "Shader.h"
#include "ErrorHandling.h"

template class Shader<ID3D11VertexShader, ShaderProfileType::Vertex>;
template class Shader<ID3D11PixelShader, ShaderProfileType::Pixel>;

template<typename ShaderType, ShaderProfileType profile>
Shader<ShaderType, profile>::Shader(ID3D11Device* device, const char* filePath, const char* entryPoint, const D3D_SHADER_MACRO* defines) {
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(
            reinterpret_cast<LPCWSTR>(filePath),
            defines,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entryPoint,
            profile == ShaderProfileType::Pixel ? PIXEL_PROFILE : VERTEX_PROFILE,
            flags,
            0,
            shaderBlob.GetAddressOf(),
            &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
            errorBlob->Release();
        }

        if (shaderBlob) {
            shaderBlob->Release();
        }

        DX("Failed to compile shader", hr);
        return;
    }

    if (errorBlob) {
        errorBlob->Release();
    }

    if (profile == ShaderProfileType::Pixel) {
        DX("Failed to create pixel shader", device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, shader.GetAddressOf()));
    } else {
        DX("Failed to create vertex shader", device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, shader.GetAddressOf()));
    }
};

template<typename ShaderType, ShaderProfileType profile>
ShaderType* Shader<ShaderType, profile>::GetShader() const {
    return shader.Get();
}