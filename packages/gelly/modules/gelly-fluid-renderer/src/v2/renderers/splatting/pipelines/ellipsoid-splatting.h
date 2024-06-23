#ifndef ELLIPSOID_SPLATTING_H
#define ELLIPSOID_SPLATTING_H

#include <device.h>
#include <helpers/create-gsc-shader.h>
#include <pipeline/pipeline.h>

#include <memory>
#include <optional>

#include "SplattingGS.h"
#include "SplattingPS.h"
#include "SplattingVS.h"
#include "pipeline-info.h"

namespace gelly {
namespace renderer {
namespace splatting {

inline auto CreateEllipsoidSplattingPipeline(const PipelineInfo &info)
	-> std::shared_ptr<Pipeline> {
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
				.width = info.width,
				.height = info.height,
				.minDepth = 0.0f,
				.maxDepth = 1.0f,
			},
		.rasterizerState =
			{
				.fillMode = D3D11_FILL_SOLID,
				.cullMode = D3D11_CULL_NONE,
			}
	});

	const auto vertexShader = VS_FROM_GSC(SplattingVS, info.device);

	const auto inputLayout =
		std::make_shared<InputLayout>(InputLayout::InputLayoutCreateInfo{
			.device = info.device,
			.vertexShader = vertexShader,
			.inputElements =
				{
					D3D11_INPUT_ELEMENT_DESC{
						.SemanticName = "SV_POSITION",
						.SemanticIndex = 0,
						.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
						.InputSlot = 0,
						.AlignedByteOffset = 0,
						.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
						.InstanceDataStepRate = 0
					},
					D3D11_INPUT_ELEMENT_DESC{
						.SemanticName = "ANISOTROPY",
						.SemanticIndex = 0,
						.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
						.InputSlot = 1,
						.AlignedByteOffset = 0,
						.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
						.InstanceDataStepRate = 0
					},
					D3D11_INPUT_ELEMENT_DESC{
						.SemanticName = "ANISOTROPY",
						.SemanticIndex = 1,
						.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
						.InputSlot = 2,
						.AlignedByteOffset = 0,
						.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
						.InstanceDataStepRate = 0
					},
					D3D11_INPUT_ELEMENT_DESC{
						.SemanticName = "ANISOTROPY",
						.SemanticIndex = 2,
						.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
						.InputSlot = 3,
						.AlignedByteOffset = 0,
						.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
						.InstanceDataStepRate = 0
					},
				},
		});

	// copy elision will make it so this doesn't expire immediately
	// after the return
	return Pipeline::CreatePipeline(
		{.name = "Splat ellipsoid depth",
		 .device = info.device,
		 .renderPass = renderPass,
		 .inputLayout = inputLayout,
		 .primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		 .inputs =
			 {
				 InputVertexBuffer{
					 .vertexBuffer = info.internalBuffers->particlePositions,
					 .slot = 0
				 },
				 InputVertexBuffer{
					 .vertexBuffer = info.internalBuffers->anisotropyQ1,
					 .slot = 1
				 },
				 InputVertexBuffer{
					 .vertexBuffer = info.internalBuffers->anisotropyQ2,
					 .slot = 2
				 },
				 InputVertexBuffer{
					 .vertexBuffer = info.internalBuffers->anisotropyQ3,
					 .slot = 3
				 },
			 },
		 .outputs = {OutputTexture{
			 .texture = info.internalTextures->unfilteredEllipsoidDepth,
			 .bindFlag = D3D11_BIND_RENDER_TARGET,
			 .slot = 0,
			 .clearColor = {1.f, 0.f, 0.f, 0.f},
		 }},
		 .shaderGroup =
			 {.pixelShader = PS_FROM_GSC(SplattingPS, info.device),
			  .vertexShader = vertexShader,
			  .geometryShader = {GS_FROM_GSC(SplattingGS, info.device)},
			  .constantBuffers =
				  {info.internalBuffers->fluidRenderCBuffer.GetBuffer()}},
		 .depthBuffer = {info.internalTextures->ellipsoidDepthBuffer}}
	);
}

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly
#endif	// ELLIPSOID_SPLATTING_H
