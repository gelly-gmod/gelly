#include "Shaders.h"

#include <d3d11.h>

#include <cstdio>
#include <stdexcept>

#include "Logging.h"

using namespace testbed;

ShaderBuffer testbed::LoadShaderBytecodeFromFile(const char *filepath) {
	ShaderBuffer buffer = {};

	FILE *file = fopen(filepath, "rb");
	if (!file) {
		GetLogger()->Error("Failed to open file");
		return buffer;
	}

	fseek(file, 0, SEEK_END);
	buffer.size = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer.buffer = malloc(buffer.size);
	fread(buffer.buffer, buffer.size, 1, file);

	fclose(file);

	return buffer;
}

void testbed::FreeShaderBuffer(ShaderBuffer buffer) { free(buffer.buffer); }

ID3D11PixelShader *testbed::GetPixelShaderFromFile(
	ID3D11Device *device, const char *filepath
) {
	ShaderBuffer buffer = LoadShaderBytecodeFromFile(filepath);
	if (!buffer.buffer) {
		GetLogger()->Error("Failed to load shader bytecode");
		return nullptr;
	}

	ID3D11PixelShader *shader = nullptr;
	HRESULT result =
		device->CreatePixelShader(buffer.buffer, buffer.size, nullptr, &shader);
	if (FAILED(result)) {
		GetLogger()->Error("Failed to create pixel shader");
		return nullptr;
	}

	FreeShaderBuffer(buffer);

	return shader;
}

ID3D11VertexShader *testbed::GetVertexShaderFromFile(
	ID3D11Device *device, const char *filepath
) {
	ShaderBuffer buffer = LoadShaderBytecodeFromFile(filepath);
	if (!buffer.buffer) {
		GetLogger()->Error("Failed to load shader bytecode");
		return nullptr;
	}

	ID3D11VertexShader *shader = nullptr;
	HRESULT result = device->CreateVertexShader(
		buffer.buffer, buffer.size, nullptr, &shader
	);
	if (FAILED(result)) {
		GetLogger()->Error("Failed to create vertex shader");
		return nullptr;
	}

	FreeShaderBuffer(buffer);

	return shader;
}