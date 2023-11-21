#ifndef IMANAGEDSHADER_H
#define IMANAGEDSHADER_H

#include <GellyInterface.h>

enum class ShaderType {
	Vertex,
	Pixel,
};

class IRenderContext;

gelly_interface IManagedShader {
public:
	virtual ~IManagedShader() = 0;

	virtual void AttachToContext(GellyObserverPtr<IRenderContext> context) = 0;
	virtual void SetBytecode(const uint8_t *bytecode, size_t bytecodeSize) = 0;

	virtual const uint8_t *GetBytecode() = 0;
	virtual size_t GetBytecodeSize() = 0;

	virtual void SetType(ShaderType type) = 0;
	virtual ShaderType GetType() = 0;

	virtual void Create() = 0;
	virtual void Destroy() = 0;

	virtual void Bind() = 0;
};

#endif	// IMANAGEDSHADER_H
