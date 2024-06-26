#include "detail/d3d11/Shaders.h"

#include "detail/d3d11/ErrorHandling.h"

namespace d3d11 {
ID3DBlob *compile_shader(
	const ShaderCompileOptions &options, const char *profile
) {
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	HRESULT compileResult = D3DCompile(
		options.shader.buffer,
		options.shader.size,
		options.shader.name,
		options.defines,
		nullptr,
		options.shader.entryPoint,
		profile,
		flags,
		0,
		shaderBlob.GetAddressOf(),
		errorBlob.GetAddressOf()
	);

	if (FAILED(compileResult)) {
		// Write the error messages so
		// that the developer can see
		// them before we terminate.
		printf(
			"Failed to compile shader: %s\n",
			(char *)errorBlob->GetBufferPointer()
		);
		DX("Failed to compile shader", compileResult);
	}

	// Transfers ownership.
	return shaderBlob.Detach();
}

ShaderCompileResult<ID3D11PixelShader> compile_pixel_shader(
	const ShaderCompileOptions &options
) {
	ID3D11PixelShader *shader;
	ID3DBlob *shaderBlob = compile_shader(options, D3D11_PIXEL_PROFILE);

	DX("Failed to create pixel shader",
	   options.device->CreatePixelShader(
		   shaderBlob->GetBufferPointer(),
		   shaderBlob->GetBufferSize(),
		   nullptr,
		   &shader
	   ));

	return {shaderBlob, shader};
}

ShaderCompileResult<ID3D11VertexShader> compile_vertex_shader(
	const ShaderCompileOptions &options
) {
	ID3D11VertexShader *shader;
	ID3DBlob *shaderBlob = compile_shader(options, D3D11_VERTEX_PROFILE);

	DX("Failed to create vertex shader",
	   options.device->CreateVertexShader(
		   shaderBlob->GetBufferPointer(),
		   shaderBlob->GetBufferSize(),
		   nullptr,
		   &shader
	   ));

	return {shaderBlob, shader};
}

ShaderCompileResult<ID3D11GeometryShader> compile_geometry_shader(
	const ShaderCompileOptions &options
) {
	ID3D11GeometryShader *shader;
	ID3DBlob *shaderBlob = compile_shader(options, D3D11_GEOMETRY_PROFILE);

	DX("Failed to create geometry shader",
	   options.device->CreateGeometryShader(
		   shaderBlob->GetBufferPointer(),
		   shaderBlob->GetBufferSize(),
		   nullptr,
		   &shader
	   ));

	return {shaderBlob, shader};
}

ShaderCompileResult<ID3D11ComputeShader> compile_compute_shader(
	const ShaderCompileOptions &options
) {
	ID3D11ComputeShader *shader;
	ID3DBlob *shaderBlob = compile_shader(options, D3D11_COMPUTE_PROFILE);

	DX("Failed to create compute shader",
	   options.device->CreateComputeShader(
		   shaderBlob->GetBufferPointer(),
		   shaderBlob->GetBufferSize(),
		   nullptr,
		   &shader
	   ));

	return {shaderBlob, shader};
}

};	// namespace splatting