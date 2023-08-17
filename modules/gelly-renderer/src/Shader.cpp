#include "detail/Shader.h"
#include "detail/ErrorHandling.h"

template class Shader<ID3D11VertexShader>;
template class Shader<ID3D11PixelShader>;

template<> Shader<ID3D11PixelShader>::Shader(ID3D11Device* device, const wchar_t* filePath, const char* entryPoint, const D3D_SHADER_MACRO* defines) {
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
            PIXEL_PROFILE,
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

    DX("Failed to create pixel shader", device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, shader.GetAddressOf()));
};

template<> Shader<ID3D11VertexShader>::Shader(ID3D11Device* device, const wchar_t* filePath, const char* entryPoint, const D3D_SHADER_MACRO* defines) {
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
            VERTEX_PROFILE,
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

    DX("Failed to create vertex shader", device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, shader.GetAddressOf()));
};

template<typename ShaderType>
ShaderType* Shader<ShaderType>::GetShader() const {
    return shader.Get();
}

template<typename ShaderType>
ID3DBlob* Shader<ShaderType>::GetShaderBlob() const {
    return shaderBlob.Get();
}