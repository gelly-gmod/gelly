#include "screen-quad.h"

#include "ScreenQuadVS.h"
#include "helpers/create-gsc-shader.h"

namespace gelly {
namespace renderer {
namespace util {

static ScreenQuad::Vertex screenQuadVertices[4] = {
	{-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
	{-1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},
	{1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f},
};

ScreenQuad::ScreenQuad(const ScreenQuadCreateInfo &createInfo) :
	createInfo(createInfo),
	vertexBuffer(CreateVertexBuffer()),
	vertexShader(CreateVertexShader()),
	inputLayout(nullptr) {
	// it's imperative that we actually have a vertex shader, construction order
	// is not guaranteed
	inputLayout = CreateInputLayout(vertexShader);
}

auto ScreenQuad::GetVertexBuffer() const -> InputVertexBuffer {
	return InputVertexBuffer{
		.vertexBuffer = vertexBuffer,
		.slot = 0,
	};
}

auto ScreenQuad::GetInputLayout() const -> std::shared_ptr<InputLayout> {
	return inputLayout;
}

auto ScreenQuad::GetVertexShader() const -> std::shared_ptr<VertexShader> {
	return vertexShader;
}

auto ScreenQuad::CreateVertexBuffer() -> std::shared_ptr<Buffer> {
	return Buffer::CreateBuffer(
		Buffer::BufferCreateInfo::WithAutomaticStride<Vertex>(
			{.device = createInfo.device,
			 .maxElementCount = 4,
			 .initialData = reinterpret_cast<const void *>(screenQuadVertices),
			 .usage = D3D11_USAGE_IMMUTABLE,
			 .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
			 .bindFlags = D3D11_BIND_VERTEX_BUFFER}
		)
	);
}

auto ScreenQuad::CreateVertexShader() -> std::shared_ptr<VertexShader> {
	return VS_FROM_GSC(ScreenQuadVS, createInfo.device);
}

auto ScreenQuad::CreateInputLayout(const std::shared_ptr<VertexShader> &shader)
	-> std::shared_ptr<InputLayout> {
	return std::make_shared<InputLayout>(InputLayout::InputLayoutCreateInfo{
		.device = createInfo.device,
		.vertexShader = shader,
		.inputElements =
			{D3D11_INPUT_ELEMENT_DESC{
				 .SemanticName = "SV_POSITION",
				 .SemanticIndex = 0,
				 .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				 .InputSlot = 0,
				 .AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
				 .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
				 .InstanceDataStepRate = 0,
			 },
			 D3D11_INPUT_ELEMENT_DESC{
				 .SemanticName = "TEXCOORD",
				 .SemanticIndex = 0,
				 .Format = DXGI_FORMAT_R32G32_FLOAT,
				 .InputSlot = 0,
				 .AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
				 .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
				 .InstanceDataStepRate = 0,
			 }}
	});
}

}  // namespace util
}  // namespace renderer
}  // namespace gelly