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

#undef min
#undef max

namespace gelly {
namespace renderer {
namespace splatting {

inline auto CreateEllipsoidSplattingPipeline(
	const PipelineInfo &info, const size_t frameIndex, float outputScale = 1.f
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
			.renderTarget =
				{D3D11_RENDER_TARGET_BLEND_DESC{
					 .BlendEnable = true,  // we do a no-op blend for absorption
					 .SrcBlend = D3D11_BLEND_ONE,
					 .DestBlend = D3D11_BLEND_ZERO,
					 .BlendOp = D3D11_BLEND_OP_ADD,
					 .SrcBlendAlpha = D3D11_BLEND_ONE,
					 .DestBlendAlpha = D3D11_BLEND_ZERO,
					 .BlendOpAlpha = D3D11_BLEND_OP_ADD,
					 .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL
				 },
				 D3D11_RENDER_TARGET_BLEND_DESC{
					 // we can use min/max blend ops to actually depth test
					 // manually which is important for the calculating front
					 // and back
					 // depth efficiently
					 .BlendEnable = true,
					 .SrcBlend = D3D11_BLEND_ONE,
					 .DestBlend = D3D11_BLEND_ONE,
					 .BlendOp = D3D11_BLEND_OP_MIN,
					 .SrcBlendAlpha = D3D11_BLEND_ONE,
					 .DestBlendAlpha = D3D11_BLEND_ONE,
					 .BlendOpAlpha = D3D11_BLEND_OP_MIN,
					 .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED |
											  D3D11_COLOR_WRITE_ENABLE_GREEN
				 },
				 D3D11_RENDER_TARGET_BLEND_DESC{
					 // thickness, we just want to add the values
					 .BlendEnable = false,
					 .SrcBlend = D3D11_BLEND_ONE,
					 .DestBlend = D3D11_BLEND_ZERO,
					 .BlendOp = D3D11_BLEND_OP_ADD,
					 .SrcBlendAlpha = D3D11_BLEND_ONE,
					 .DestBlendAlpha = D3D11_BLEND_ZERO,
					 .BlendOpAlpha = D3D11_BLEND_OP_ADD,
					 .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
				 }},
		}},
		.outputScale = outputScale
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
			 {InputBuffer{
				  .buffer = info.internalBuffers[0]->particleAbsorptions,
				  .bindFlag = D3D11_BIND_SHADER_RESOURCE,
				  .slot = 0
			  },
			  InputBuffer{
				  .buffer =
					  info.internalBuffers[frameIndex]->particleAccelerations,
				  .bindFlag = D3D11_BIND_SHADER_RESOURCE,
				  .slot = 1
			  },
			  InputVertexBuffer{
				  .vertexBuffer =
					  info.internalBuffers[frameIndex]->particlePositions,
				  .slot = 0
			  },
			  InputVertexBuffer{
				  .vertexBuffer =
					  info.internalBuffers[frameIndex]->anisotropyQ1,
				  .slot = 1
			  },
			  InputVertexBuffer{
				  .vertexBuffer =
					  info.internalBuffers[frameIndex]->anisotropyQ2,
				  .slot = 2
			  },
			  InputVertexBuffer{
				  .vertexBuffer =
					  info.internalBuffers[frameIndex]->anisotropyQ3,
				  .slot = 3
			  }},
		 .outputs =
			 {OutputTexture{
				  .texture =
					  info.internalTextures[frameIndex]->unfilteredAlbedo,
				  .bindFlag = D3D11_BIND_RENDER_TARGET,
				  .slot = 0,
				  .clearColor = {0.f, 0.f, 0.f, 0.f},
			  },
			  OutputTexture{
				  .texture = info.outputTextures[frameIndex]->ellipsoidDepth,
				  .bindFlag = D3D11_BIND_RENDER_TARGET,
				  .slot = 1,
				  .clearColor = {1.f, D3D11_FLOAT32_MAX, 1.f, 1.f}
			  },
			  OutputTexture{
				  .texture =
					  info.internalTextures[frameIndex]->unfilteredThickness,
				  .bindFlag = D3D11_BIND_RENDER_TARGET,
				  .slot = 2,
				  .clearColor = {0.f, 0.f, 0.f, 0.f},
				  .clear = true
			  }},
		 .shaderGroup =
			 {.pixelShader = PS_FROM_GSC(SplattingPS, info.device),
			  .vertexShader = vertexShader,
			  .geometryShader = {GS_FROM_GSC(SplattingGS, info.device)},
			  .constantBuffers = {info.internalBuffers[frameIndex]
									  ->fluidRenderCBuffer.GetBuffer()}},
		 .depthBuffer = info.internalTextures[frameIndex]->ellipsoidDepthBuffer,
		 .defaultVertexCount = 0}
	);
}

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly
#endif	// ELLIPSOID_SPLATTING_H
