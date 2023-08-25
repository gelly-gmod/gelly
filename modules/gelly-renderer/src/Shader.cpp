#include "detail/Shader.h"

#include "detail/ErrorHandling.h"

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

	return shaderBlob.Get();
}

ShaderCompileResult<ID3D11PixelShader> compile_pixel_shader(
	const ShaderCompileOptions &options
) {
	ComPtr<ID3D11PixelShader> shader;
	ComPtr<ID3DBlob> shaderBlob = compile_shader(options, PIXEL_PROFILE);

	DX("Failed to create pixel shader",
	   options.device->CreatePixelShader(
		   shaderBlob->GetBufferPointer(),
		   shaderBlob->GetBufferSize(),
		   nullptr,
		   shader.GetAddressOf()
	   ));

	return {shaderBlob.Get(), shader.Get()};
}

ShaderCompileResult<ID3D11VertexShader> compile_vertex_shader(
	const ShaderCompileOptions &options
) {
	ComPtr<ID3D11VertexShader> shader;
	ComPtr<ID3DBlob> shaderBlob = compile_shader(options, VERTEX_PROFILE);

	DX("Failed to create vertex shader",
	   options.device->CreateVertexShader(
		   shaderBlob->GetBufferPointer(),
		   shaderBlob->GetBufferSize(),
		   nullptr,
		   shader.GetAddressOf()
	   ));

	return {shaderBlob.Get(), shader.Get()};
}

ShaderCompileResult<ID3D11GeometryShader> compile_geometry_shader(
	const ShaderCompileOptions &options
) {
	ComPtr<ID3D11GeometryShader> shader;
	ComPtr<ID3DBlob> shaderBlob = compile_shader(options, GEOMETRY_PROFILE);
	
	DX("Failed to create geometry shader",
	   options.device->CreateGeometryShader(
		   shaderBlob->GetBufferPointer(),
		   shaderBlob->GetBufferSize(),
		   nullptr,
		   shader.GetAddressOf()
	   ));

	return {shaderBlob.Get(), shader.Get()};
}
