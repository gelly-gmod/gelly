#ifndef GELLY_D3D9SHADERS_H
#define GELLY_D3D9SHADERS_H

#define D3D9_PIXEL_PROFILE "ps_3_0"
#define D3D9_VERTEX_PROFILE "vs_3_0"

#include <d3d9.h>
#include <d3dcompiler.h>

namespace d3d9 {
struct ShaderCompileOptions {
	IDirect3DDevice9 *device;
	struct {
		void *buffer;
		size_t size;
		const char *name;
		const char *entryPoint;
	} shader;
	const D3D_SHADER_MACRO *defines;
};

IDirect3DPixelShader9 *compile_pixel_shader(const ShaderCompileOptions &options
);
IDirect3DVertexShader9 *compile_vertex_shader(
	const ShaderCompileOptions &options
);
}  // namespace d3d9

#endif	// GELLY_D3D9SHADERS_H