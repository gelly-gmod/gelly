#ifndef INPUT_GROUP_H
#define INPUT_GROUP_H
#include <helpers/comptr.h>

#include <memory>

#include "buffer.h"
#include "shader.h"

namespace gelly {
namespace renderer {

class InputLayout {
public:
	struct InputLayoutCreateInfo {
		const std::shared_ptr<Device> device;
		const std::shared_ptr<VertexShader> vertexShader;
		D3D11_INPUT_ELEMENT_DESC inputElementDescs[8];
	};

	InputLayout(const InputLayoutCreateInfo &createInfo);
	~InputLayout() = default;

	auto GetInputLayout() -> ComPtr<ID3D11InputLayout>;
	auto GetVertexShader() -> std::shared_ptr<VertexShader>;

private:
	InputLayoutCreateInfo createInfo;
	ComPtr<ID3D11InputLayout> inputLayout;

	auto CreateInputLayout() -> ComPtr<ID3D11InputLayout>;
};

}  // namespace renderer
}  // namespace gelly

#endif	// INPUT_GROUP_H
