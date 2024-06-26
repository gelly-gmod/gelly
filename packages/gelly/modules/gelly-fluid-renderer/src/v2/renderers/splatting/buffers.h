#ifndef BUFFERS_H
#define BUFFERS_H
#include <memory>

#include "cbuffers.h"
#include "resources/buffer.h"

namespace gelly {
namespace renderer {
namespace splatting {
struct float3 {
	float x;
	float y;
	float z;
};

struct float4 {
	float x;
	float y;
	float z;
	float w;
};

struct InternalBuffers {
	using BufferCreateInfo = Buffer::BufferCreateInfo;

	std::shared_ptr<Buffer> particlePositions = nullptr;
	std::shared_ptr<Buffer> particleAbsorptions = nullptr;

	std::shared_ptr<Buffer> anisotropyQ1 = nullptr;
	std::shared_ptr<Buffer> anisotropyQ2 = nullptr;
	std::shared_ptr<Buffer> anisotropyQ3 = nullptr;

	std::shared_ptr<Buffer> foamPositions = nullptr;
	std::shared_ptr<Buffer> foamVelocities = nullptr;

	cbuffer::FluidRenderCBuffer fluidRenderCBuffer;

	InternalBuffers(
		const std::shared_ptr<Device> &device, const unsigned int maxParticles
	) :
		particlePositions(
			Buffer::CreateBuffer(BufferCreateInfo::WithAutomaticStride<float4>(
				{.device = device,
				 .maxElementCount = maxParticles,
				 .initialData = nullptr,
				 .usage = D3D11_USAGE_DEFAULT,
				 .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				 .bindFlags = D3D11_BIND_VERTEX_BUFFER}
			))
		),
		particleAbsorptions(
			Buffer::CreateBuffer(BufferCreateInfo::WithAutomaticStride<float3>(
				{.device = device,
				 .maxElementCount = maxParticles,
				 .initialData = nullptr,
				 .usage = D3D11_USAGE_DEFAULT,
				 .format = DXGI_FORMAT_R32G32B32_FLOAT,
				 .bindFlags = D3D11_BIND_SHADER_RESOURCE}
			))
		),
		anisotropyQ1(
			Buffer::CreateBuffer(BufferCreateInfo::WithAutomaticStride<float4>(
				{.device = device,
				 .maxElementCount = maxParticles,
				 .initialData = nullptr,
				 .usage = D3D11_USAGE_DEFAULT,
				 .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				 .bindFlags = D3D11_BIND_VERTEX_BUFFER}
			))
		),
		anisotropyQ2(
			Buffer::CreateBuffer(BufferCreateInfo::WithAutomaticStride<float4>(
				{.device = device,
				 .maxElementCount = maxParticles,
				 .initialData = nullptr,
				 .usage = D3D11_USAGE_DEFAULT,
				 .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				 .bindFlags = D3D11_BIND_VERTEX_BUFFER}
			))
		),
		anisotropyQ3(
			Buffer::CreateBuffer(BufferCreateInfo::WithAutomaticStride<float4>(
				{.device = device,
				 .maxElementCount = maxParticles,
				 .initialData = nullptr,
				 .usage = D3D11_USAGE_DEFAULT,
				 .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				 .bindFlags = D3D11_BIND_VERTEX_BUFFER}
			))
		),
		foamPositions(
			Buffer::CreateBuffer(BufferCreateInfo::WithAutomaticStride<float4>(
				{.device = device,
				 .maxElementCount = maxParticles,
				 .initialData = nullptr,
				 .usage = D3D11_USAGE_DEFAULT,
				 .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				 .bindFlags = D3D11_BIND_VERTEX_BUFFER}
			))
		),
		foamVelocities(
			Buffer::CreateBuffer(BufferCreateInfo::WithAutomaticStride<float4>(
				{.device = device,
				 .maxElementCount = maxParticles,
				 .initialData = nullptr,
				 .usage = D3D11_USAGE_DEFAULT,
				 .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				 .bindFlags = D3D11_BIND_VERTEX_BUFFER}
			))
		),
		fluidRenderCBuffer({.device = device}){};
};

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly

#endif	// BUFFERS_H
