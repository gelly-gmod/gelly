#include "D3D9Shader.h"

#include <d3dcompiler.h>
#include <wrl.h>

#include "ErrorHandling.h"

using namespace Microsoft::WRL;

ID3DBlob *compile_shader(
	const d3d9::ShaderCompileOptions &options, const char *profile
) {
	ID3DBlob *shaderBlob;
	ID3DBlob *errorBlob;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	HRESULT hr = D3DCompile(
		options.shader.buffer,
		options.shader.size,
		options.shader.name,
		options.defines,
		nullptr,
		options.shader.entryPoint,
		profile,
		0,
		0,
		&shaderBlob,
		&errorBlob
	);

	if (FAILED(hr)) {
		// Print debug info using printf
		printf("Failed to compile shader %s\n", options.shader.name);
		printf("Error blob: %s\n", (char *)errorBlob->GetBufferPointer());
	}

	DX("Failed to compile shader", hr);

	return shaderBlob;
}

IDirect3DPixelShader9 *d3d9::compile_pixel_shader(
	const ShaderCompileOptions &options
) {
	// ComPtr will take care of cleaning up the shader blob
	ComPtr<ID3DBlob> shaderBlob;
	shaderBlob.Attach(compile_shader(options, PIXEL_PROFILE));

	IDirect3DPixelShader9 *shader;
	DX("Failed to create pixel shader",
	   options.device->CreatePixelShader(
		   (DWORD *)shaderBlob->GetBufferPointer(), &shader
	   ));

	return shader;
}

IDirect3DVertexShader9 *d3d9::compile_vertex_shader(
	const ShaderCompileOptions &options
) {
	ComPtr<ID3DBlob> shaderBlob;
	shaderBlob.Attach(compile_shader(options, VERTEX_PROFILE));

	IDirect3DVertexShader9 *shader;
	DX("Failed to create vertex shader",
	   options.device->CreateVertexShader(
		   (DWORD *)shaderBlob->GetBufferPointer(), &shader
	   ));

	return shader;
}