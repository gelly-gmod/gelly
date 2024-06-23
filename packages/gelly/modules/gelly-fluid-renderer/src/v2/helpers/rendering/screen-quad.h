#ifndef SCREEN_QUAD_H
#define SCREEN_QUAD_H
#include <memory>

#include "pipeline/binding/input.h"
#include "resources/buffer.h"
#include "resources/input-layout.h"

namespace gelly {
namespace renderer {
namespace util {

class ScreenQuad {
public:
	struct ScreenQuadCreateInfo {
		std::shared_ptr<Device> device;
	};

	explicit ScreenQuad(const ScreenQuadCreateInfo &createInfo);
	~ScreenQuad() = default;

	[[nodiscard]] auto GetVertexBuffer() const -> InputVertexBuffer;
	[[nodiscard]] auto GetInputLayout() const -> std::shared_ptr<InputLayout>;
	[[nodiscard]] auto GetVertexShader() const -> std::shared_ptr<VertexShader>;

	static auto GetPrimitiveTopology() -> D3D11_PRIMITIVE_TOPOLOGY {
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	}

private:
	struct Vertex {
		float x, y, z, w;
		float u, v;
	};

	constexpr Vertex screenQuadVertices[4] = {
		{-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
		{-1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f},
	};

	ScreenQuadCreateInfo createInfo;

	std::shared_ptr<Buffer> vertexBuffer;
	std::shared_ptr<InputLayout> inputLayout;
	std::shared_ptr<VertexShader> vertexShader;

	auto CreateVertexBuffer() -> std::shared_ptr<Buffer>;
	auto CreateVertexShader() -> std::shared_ptr<VertexShader>;
	auto CreateInputLayout(const std::shared_ptr<VertexShader> &shader)
		-> std::shared_ptr<InputLayout>;
};

}  // namespace util
}  // namespace renderer
}  // namespace gelly

#endif	// SCREEN_QUAD_H
