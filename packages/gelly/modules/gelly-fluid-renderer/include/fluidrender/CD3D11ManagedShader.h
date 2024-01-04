#ifndef CD3D11MANAGEDSHADER_H
#define CD3D11MANAGEDSHADER_H

#include <d3d11.h>

#include <variant>

#include "IManagedShader.h"
#include "IRenderContext.h"

class CD3D11ManagedShader : public IManagedShader {
private:
	std::variant<
		std::monostate,
		ID3D11VertexShader *,
		ID3D11PixelShader *,
		ID3D11GeometryShader *,
		ID3D11ComputeShader *>
		shader;

	GellyObserverPtr<IRenderContext> context;
	ShaderType type;

	const uint8_t *bytecode;
	size_t bytecodeSize;

	[[nodiscard]] bool IsEmpty() const;

public:
	CD3D11ManagedShader();
	~CD3D11ManagedShader() override;

	void AttachToContext(GellyObserverPtr<IRenderContext> context) override;
	void SetBytecode(const uint8_t *bytecode, size_t bytecodeSize) override;

	const uint8_t *GetBytecode() override;
	size_t GetBytecodeSize() override;

	void SetType(ShaderType type) override;
	ShaderType GetType() override;

	void Create() override;
	void Destroy() override;

	void Bind() override;
};
#endif	// CD3D11MANAGEDSHADER_H
