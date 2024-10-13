#ifndef NORMAL_ESTIMATION_H
#define NORMAL_ESTIMATION_H

#include <device.h>
#include <helpers/create-gsc-shader.h>
#include <pipeline/pipeline.h>

#include <memory>
#include <optional>

#include "EstimateNormalPS.h"
#include "helpers/rendering/screen-quad.h"
#include "pipeline-info.h"

namespace gelly {
namespace renderer {
namespace splatting {
inline auto CreateNormalEstimationPipeline(
	const PipelineInfo &info,
	const std::shared_ptr<Texture> &inputDepth,
	const std::shared_ptr<Texture> &outputNormal,
	bool outputPositions = true
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
			{.fillMode = D3D11_FILL_SOLID, .cullMode = D3D11_CULL_NONE}
	});

	const util::ScreenQuad screenQuad({.device = info.device});

	return Pipeline::CreatePipeline(
		{.name = "Estimating normals",
		 .device = info.device,
		 .renderPass = renderPass,
		 .inputLayout = screenQuad.GetInputLayout(),
		 .primitiveTopology = util::ScreenQuad::GetPrimitiveTopology(),
		 .inputs =
			 {screenQuad.GetVertexBuffer(),
			  InputTexture{
				  .texture = inputDepth,
				  .bindFlag = D3D11_BIND_SHADER_RESOURCE,
				  .slot = 0
			  }},
		 .outputs = {OutputTexture{
			 .texture = outputNormal,
			 .bindFlag = D3D11_BIND_RENDER_TARGET,
			 .slot = 0,
			 .clearColor = {0.f, 0.f, 0.f, 0.f}
		 }},
		 .shaderGroup =
			 {.pixelShader = PS_FROM_GSC(EstimateNormalPS, info.device),
			  .vertexShader = screenQuad.GetVertexShader(),
			  .constantBuffers =
				  {info.internalBuffers->fluidRenderCBuffer.GetBuffer()}},
		 .depthBuffer = std::nullopt,
		 .defaultVertexCount = 4}
	);
}
}  // namespace splatting
}  // namespace renderer
}  // namespace gelly

#endif	// NORMAL_ESTIMATION_H
