#ifndef GELLY_SHADERS_H
#define GELLY_SHADERS_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>

using namespace Microsoft::WRL;

#define D3D11_VERTEX_PROFILE "vs_5_0"
#define D3D11_PIXEL_PROFILE "ps_5_0"
#define D3D11_GEOMETRY_PROFILE "gs_5_0"
#define D3D11_COMPUTE_PROFILE "cs_5_0"

namespace d3d11 {
template <typename Shader>
struct ShaderCompileResult {
	ID3DBlob *shaderBlob;
	Shader *shader;
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
ShaderCompileResult<ID3D11ComputeShader> compile_compute_shader(
	const ShaderCompileOptions &options
);

}  // namespace splatting

#endif	// GELLY_SHADERS_H
