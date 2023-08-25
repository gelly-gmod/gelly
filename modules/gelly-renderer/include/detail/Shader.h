#ifndef GELLY_SHADER_H
#define GELLY_SHADER_H

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>

using namespace Microsoft::WRL;

#define VERTEX_PROFILE "vs_5_0"
#define PIXEL_PROFILE "ps_5_0"
#define GEOMETRY_PROFILE "gs_5_0"

template <typename Shader>
struct ShaderCompileResult {
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<Shader> shader;
};

struct ShaderCompileOptions {
	ID3D11Device *device;
	struct {
		void *buffer;
		size_t size;
		const char *name;
		const char *entryPoint;
	} shader;
	const D3D_SHADER_MACRO *defines;
};

ShaderCompileResult<ID3D11PixelShader> compile_pixel_shader(
	const ShaderCompileOptions &options
);
ShaderCompileResult<ID3D11VertexShader> compile_vertex_shader(
	const ShaderCompileOptions &options
);
ShaderCompileResult<ID3D11GeometryShader> compile_geometry_shader(
	const ShaderCompileOptions &options
);

#endif	// GELLY_SHADER_H
