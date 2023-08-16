#ifndef GELLY_SHADER_H
#define GELLY_SHADER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <windows.h>
#include <wrl.h>

using namespace Microsoft::WRL;

#define VERTEX_PROFILE "vs_5_0"
#define PIXEL_PROFILE "ps_5_0"

enum class ShaderProfileType {
    Vertex,
    Pixel
};

template<typename ShaderType, ShaderProfileType profile>
class Shader {
private:
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ShaderType> shader;
public:
    Shader(ID3D11Device* device, const char* filePath, const char* entryPoint, const D3D_SHADER_MACRO* defines);
    ~Shader() = default;

    [[nodiscard]] ShaderType* GetShader() const;
};

using VertexShader = Shader<ID3D11VertexShader, ShaderProfileType::Vertex>;
using PixelShader = Shader<ID3D11PixelShader, ShaderProfileType::Pixel>;

#endif //GELLY_SHADER_H
