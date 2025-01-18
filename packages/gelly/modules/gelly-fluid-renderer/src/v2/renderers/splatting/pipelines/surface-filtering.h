#ifndef DEPTH_FILTERING_H
#define DEPTH_FILTERING_H

#include <device.h>
#include <helpers/create-gsc-shader.h>
#include <pipeline/pipeline.h>

#include <memory>
#include <optional>

#include "FilterDepthPS.h"
#include "helpers/rendering/screen-quad.h"
#include "pipeline-info.h"

namespace gelly {
namespace renderer {
namespace splatting {
// we take in the input/output depth separately so that we are able
// to create ping pong pipelines for filter iterations
inline auto CreateSurfaceFilteringPipeline(
	const PipelineInfo &info,
	const size_t frameIndex,
	const std::shared_ptr<Texture> &inputNormal,
	const std::shared_ptr<Texture> &outputNormal,
	float scale = 0.75f
) -> std::shared_ptr<Pipeline> {
	const auto renderPass = std::make_shared<RenderPass>(RenderPass::PassInfo{
		.device = info.device,
		.depthStencilState =
			{.depthTestEnabled = false,
			 .depthWriteEnabled = false,
			 .depthComparisonFunc = D3D11_COMPARISON_ALWAYS},
		.viewportState =
			{.topLeftX = 0.f,
			 .topLeftY = 0.f,
			 .width = static_cast<float>(info.width),
			 .height = static_cast<float>(info.height),
			 .minDepth = 0.f,
			 .maxDepth = 1.f},
		.rasterizerState =
			{.fillMode = D3D11_FILL_SOLID, .cullMode = D3D11_CULL_NONE},
		.enableMipRegeneration = false,
		.outputScale = scale
	});

	const util::ScreenQuad screenQuad({.device = info.device});

	return Pipeline::CreatePipeline({
		.name = "Filtering depth",
		.device = info.device,
		.renderPass = renderPass,
		.inputLayout = screenQuad.GetInputLayout(),
		.primitiveTopology = util::ScreenQuad::GetPrimitiveTopology(),
		.inputs =
			{screenQuad.GetVertexBuffer(),
			 InputTexture{
				 .texture = info.outputTextures[frameIndex]->ellipsoidDepth,
				 .bindFlag = D3D11_BIND_SHADER_RESOURCE,
				 .slot = 0
			 },
			 InputTexture{
				 .texture = inputNormal,
				 .bindFlag = D3D11_BIND_SHADER_RESOURCE,
				 .slot = 1
			 }},
		.outputs = {OutputTexture{
			.texture = outputNormal,
			.bindFlag = D3D11_BIND_RENDER_TARGET,
			.slot = 0,
			.clear = false
		}},
		.shaderGroup =
			{.pixelShader = PS_FROM_GSC(FilterDepthPS, info.device),
			 .vertexShader = screenQuad.GetVertexShader(),
			 .constantBuffers =
				 {info.internalBuffers->fluidRenderCBuffer.GetBuffer()}},
		.depthBuffer = std::nullopt,
		.defaultVertexCount = 4,
	});
}
}  // namespace splatting
}  // namespace renderer
}  // namespace gelly

#endif	// DEPTH_FILTERING_H
