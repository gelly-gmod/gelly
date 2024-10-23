#ifndef THICKNESS_SPLATTING_H
#define THICKNESS_SPLATTING_H

#include <device.h>
#include <helpers/create-gsc-shader.h>
#include <pipeline/pipeline.h>

#include <memory>
#include <optional>

#include "SplattingGS.h"
#include "SplattingPS.h"
#include "SplattingVS.h"
#include "ThicknessGS.h"
#include "ThicknessPS.h"
#include "ThicknessVS.h"
#include "pipeline-info.h"

#undef min
#undef max

namespace gelly {
namespace renderer {
namespace splatting {

inline auto CreateThicknessSplattingPipeline(
	const PipelineInfo &info, float outputScale = 1.f
) -> std::shared_ptr<Pipeline> {
	const auto renderPass = std::make_shared<RenderPass>(RenderPass::PassInfo{
		.device = info.device,
		.depthStencilState =
			{.depthTestEnabled = false,
			 .depthWriteEnabled = false,
			 .depthComparisonFunc = D3D11_COMPARISON_LESS},
		.viewportState =
			{
				.topLeftX = 0.0f,
				.topLeftY = 0.0f,
				.width = static_cast<float>(info.width),
				.height = static_cast<float>(info.height),
				.minDepth = 0.0f,
				.maxDepth = 1.0f,
			},
		.rasterizerState =
			{
				.fillMode = D3D11_FILL_SOLID,
				.cullMode = D3D11_CULL_NONE,
			},
		.blendState = {{
			.independentBlendEnable = true,
			.renderTarget = {D3D11_RENDER_TARGET_BLEND_DESC{
				.BlendEnable = true,
				.SrcBlend = D3D11_BLEND_ONE,
				.DestBlend = D3D11_BLEND_ONE,
				.BlendOp = D3D11_BLEND_OP_ADD,
				.SrcBlendAlpha = D3D11_BLEND_ONE,
				.DestBlendAlpha = D3D11_BLEND_ONE,
				.BlendOpAlpha = D3D11_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL

			}},
		}},
		.outputScale = outputScale
	});

	const auto vertexShader = VS_FROM_GSC(ThicknessVS, info.device);

	const auto inputLayout =
		std::make_shared<InputLayout>(InputLayout::InputLayoutCreateInfo{
			.device = info.device,
			.vertexShader = vertexShader,
			.inputElements = {D3D11_INPUT_ELEMENT_DESC{
				.SemanticName = "SV_POSITION",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = 0,
				.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0
			}},
		});

	return Pipeline::CreatePipeline(
		{.name = "Splat thickness",
		 .device = info.device,
		 .renderPass = renderPass,
		 .inputLayout = inputLayout,
		 .primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		 .inputs = {InputVertexBuffer{
			 .vertexBuffer = info.internalBuffers->particlePositions, .slot = 0
		 }},
		 .outputs = {OutputTexture{
			 .texture = info.internalTextures->unfilteredThickness,
			 .bindFlag = D3D11_BIND_RENDER_TARGET,
			 .slot = 0,
			 .clearColor = {0.f, 0.f, 0.f, 0.f},
			 .clear = false
		 }},
		 .shaderGroup =
			 {.pixelShader = PS_FROM_GSC(ThicknessPS, info.device),
			  .vertexShader = vertexShader,
			  .geometryShader = {GS_FROM_GSC(ThicknessGS, info.device)},
			  .constantBuffers =
				  {info.internalBuffers->fluidRenderCBuffer.GetBuffer()}},
		 .depthBuffer = std::nullopt,
		 .defaultVertexCount = 0}
	);
}

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly
#endif	// ELLIPSOID_SPLATTING_H
