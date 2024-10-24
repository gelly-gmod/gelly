#ifndef SPRAY_SPLATTING_H
#define SPRAY_SPLATTING_H

#include <device.h>
#include <helpers/create-gsc-shader.h>
#include <pipeline/pipeline.h>

#include <memory>
#include <optional>

#include "FoamGS.h"
#include "FoamPS.h"
#include "FoamVS.h"
#include "SplattingGS.h"
#include "SplattingPS.h"
#include "SplattingVS.h"
#include "pipeline-info.h"

#undef min
#undef max

namespace gelly {
namespace renderer {
namespace splatting {

inline auto CreateSpraySplattingPipeline(
	const PipelineInfo &info, float outputScale = 1.f
) -> std::shared_ptr<Pipeline> {
	const auto renderPass = std::make_shared<RenderPass>(RenderPass::PassInfo{
		.device = info.device,
		.depthStencilState =
			{.depthTestEnabled = true,
			 .depthWriteEnabled = true,
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
				// thickness, we just want to add the values
				.BlendEnable = true,
				.SrcBlend = D3D11_BLEND_ONE,
				.DestBlend = D3D11_BLEND_ZERO,
				.BlendOp = D3D11_BLEND_OP_ADD,
				.SrcBlendAlpha = D3D11_BLEND_ONE,
				.DestBlendAlpha = D3D11_BLEND_ZERO,
				.BlendOpAlpha = D3D11_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED |
										 D3D11_COLOR_WRITE_ENABLE_GREEN |
										 D3D11_COLOR_WRITE_ENABLE_BLUE,
			}},
		}},
		.outputScale = outputScale
	});

	const auto vertexShader = VS_FROM_GSC(FoamVS, info.device);

	const auto inputLayout =
		std::make_shared<InputLayout>(InputLayout::InputLayoutCreateInfo{
			.device = info.device,
			.vertexShader = vertexShader,
			.inputElements =
				{D3D11_INPUT_ELEMENT_DESC{
					 .SemanticName = "SV_POSITION",
					 .SemanticIndex = 0,
					 .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
					 .InputSlot = 0,
					 .AlignedByteOffset = 0,
					 .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
					 .InstanceDataStepRate = 0
				 },
				 D3D11_INPUT_ELEMENT_DESC{
					 .SemanticName = "VELOCITY",
					 .SemanticIndex = 0,
					 .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
					 .InputSlot = 1,
					 .AlignedByteOffset = 0,
					 .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
					 .InstanceDataStepRate = 0
				 }},
		});

	return Pipeline::CreatePipeline(
		{.name = "Splat spray particles",
		 .device = info.device,
		 .renderPass = renderPass,
		 .inputLayout = inputLayout,
		 .primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		 .inputs =
			 {InputVertexBuffer{
				  .vertexBuffer = info.internalBuffers->foamPositions, .slot = 0
			  },
			  InputVertexBuffer{
				  .vertexBuffer = info.internalBuffers->foamVelocities,
				  .slot = 1
			  }},
		 .outputs = {OutputTexture{
			 .texture = info.internalTextures->unfilteredThickness,
			 .bindFlag = D3D11_BIND_RENDER_TARGET,
			 .slot = 0,
			 .clearColor = {0.f, 0.f, 0.f, 0.f},
			 .clear = false,
		 }},
		 .shaderGroup =
			 {.pixelShader = PS_FROM_GSC(FoamPS, info.device),
			  .vertexShader = vertexShader,
			  .geometryShader = {GS_FROM_GSC(FoamGS, info.device)},
			  .constantBuffers =
				  {info.internalBuffers->fluidRenderCBuffer.GetBuffer()}},
		 .depthBuffer = info.internalTextures->ellipsoidDepthBuffer,
		 .defaultVertexCount = 0}
	);
}

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly
#endif	// SPRAY_SPLATTING_H
