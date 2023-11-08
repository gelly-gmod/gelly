#ifndef GELLY_TESTBEDSHADERS_H
#define GELLY_TESTBEDSHADERS_H

#include <d3d11.h>

namespace testbed {
struct ShaderBuffer {
	void *buffer;
	size_t size;
};

ID3D11PixelShader *GetPixelShaderFromFile(
	ID3D11Device *device, const char *filepath
);

ID3D11VertexShader *GetVertexShaderFromFile(
	ID3D11Device *device, const char *filepath
);

ShaderBuffer LoadShaderBytecodeFromFile(const char *filepath);
void FreeShaderBuffer(ShaderBuffer buffer);

}  // namespace testbed

#endif	// GELLY_TESTBEDSHADERS_H
