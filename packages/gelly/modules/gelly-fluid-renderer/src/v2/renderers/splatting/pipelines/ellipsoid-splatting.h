#ifndef ELLIPSOID_SPLATTING_H
#define ELLIPSOID_SPLATTING_H

#include <device.h>
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

auto CreateEllipsoidSplattingPipeline(const PipelineInfo &info)
	-> std::shared_ptr<Pipeline> {
	auto renderPass = std::make_shared<RenderPass>(RenderPass::PassInfo{
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

	// copy elision will make it so this doesn't expire immediately after the
	// return
	return std::make_shared<Pipeline>(Pipeline::PipelineCreateInfo{
		.name = "Splat ellipsoid depth",
		.device = info.device,
		.renderPass = renderPass,
		.inputs = {},
		.outputs = {OutputTexture{
			.texture = info.internalTextures->unfilteredEllipsoidDepth,
			.bindFlag = D3D11_BIND_RENDER_TARGET,
			.slot = 0,
			.clearColor = {1.f, 0.f, 0.f, 0.f},
		}},
		.shaderGroup =
			{.pixelShader =
				 std::make_shared<PixelShader>(PixelShader::ShaderCreateInfo{
					 .device = info.device,
					 .shaderBlob = gsc::SplattingPS::GetBytecode(),
					 .shaderBlobSize = gsc::SplattingPS::GetBytecodeSize()
				 }),
			 .vertexShader =
				 std::make_shared<VertexShader>(VertexShader::ShaderCreateInfo{
					 .device = info.device,
					 .shaderBlob = gsc::SplattingVS::GetBytecode(),
					 .shaderBlobSize = gsc::SplattingVS::GetBytecodeSize()
				 }),
			 .geometryShader = {std::make_shared<GeometryShader>(
				 GeometryShader::ShaderCreateInfo{
					 .device = info.device,
					 .shaderBlob = gsc::SplattingGS::GetBytecode(),
					 .shaderBlobSize = gsc::SplattingGS::GetBytecodeSize()
				 }
			 )}},
		.depthBuffer = {}
	});
}

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly
#endif	// ELLIPSOID_SPLATTING_H
