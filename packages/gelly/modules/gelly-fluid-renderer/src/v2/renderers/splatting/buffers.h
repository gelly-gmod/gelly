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
	std::shared_ptr<Buffer> particleVelocities0 = nullptr;
	std::shared_ptr<Buffer> particleVelocities1 = nullptr;
	std::shared_ptr<Buffer> particleAbsorptions = nullptr;

	std::shared_ptr<Buffer> anisotropyQ1 = nullptr;
	std::shared_ptr<Buffer> anisotropyQ2 = nullptr;
	std::shared_ptr<Buffer> anisotropyQ3 = nullptr;

	std::shared_ptr<Buffer> foamPositions = nullptr;
	std::shared_ptr<Buffer> foamVelocities = nullptr;

	/**
	 * Nothing to do with acceleration structures, this is just a buffer that
	 * contains the acceleration vector for each particle, it's used for the
	 * foam rendering.
	 */
	std::shared_ptr<Buffer> particleAccelerations = nullptr;

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
				 .miscFlags = D3D11_RESOURCE_MISC_SHARED,
				 .bindFlags = D3D11_BIND_VERTEX_BUFFER}
			))
		),
		particleVelocities0(
			Buffer::CreateBuffer(BufferCreateInfo::WithAutomaticStride<float4>(
				{.device = device,
				 .maxElementCount = maxParticles,
				 .initialData = nullptr,
				 .usage = D3D11_USAGE_DEFAULT,
				 .format = DXGI_FORMAT_R32G32B32_FLOAT,
				 .miscFlags = D3D11_RESOURCE_MISC_SHARED,
				 .bindFlags = D3D11_BIND_SHADER_RESOURCE}
			))
		),
		particleVelocities1(
			Buffer::CreateBuffer(BufferCreateInfo::WithAutomaticStride<float4>(
				{.device = device,
				 .maxElementCount = maxParticles,
				 .initialData = nullptr,
				 .usage = D3D11_USAGE_DEFAULT,
				 .format = DXGI_FORMAT_R32G32B32_FLOAT,
				 .miscFlags = D3D11_RESOURCE_MISC_SHARED,
				 .bindFlags = D3D11_BIND_SHADER_RESOURCE}
			))
		),
		particleAbsorptions(
			Buffer::CreateBuffer(BufferCreateInfo::WithAutomaticStride<float3>(
				{.device = device,
				 .maxElementCount = maxParticles,
				 .initialData = nullptr,
				 .usage = D3D11_USAGE_DEFAULT,
				 .format = DXGI_FORMAT_R32G32B32_FLOAT,
				 .cpuAccessFlags = D3D11_CPU_ACCESS_WRITE,
				 .bindFlags = D3D11_BIND_SHADER_RESOURCE}
			))
		),
		particleAccelerations(
			Buffer::CreateBuffer(BufferCreateInfo::WithAutomaticStride<float>(
				{.device = device,
				 .maxElementCount = maxParticles,
				 .initialData = nullptr,
				 .usage = D3D11_USAGE_DEFAULT,
				 .format =
					 DXGI_FORMAT_R32_FLOAT,	 // We simply store the magnitude,
											 // but also know that anything else
											 // wont work since its a typed UAV.
				 .bindFlags =
					 D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS}
			))
		),
		anisotropyQ1(
			Buffer::CreateBuffer(BufferCreateInfo::WithAutomaticStride<float4>(
				{.device = device,
				 .maxElementCount = maxParticles,
				 .initialData = nullptr,
				 .usage = D3D11_USAGE_DEFAULT,
				 .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				 .miscFlags = D3D11_RESOURCE_MISC_SHARED,
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
				 .miscFlags = D3D11_RESOURCE_MISC_SHARED,
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
				 .miscFlags = D3D11_RESOURCE_MISC_SHARED,
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
				 .miscFlags = D3D11_RESOURCE_MISC_SHARED,
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
				 .miscFlags = D3D11_RESOURCE_MISC_SHARED,
				 .bindFlags = D3D11_BIND_VERTEX_BUFFER}
			))
		),

		fluidRenderCBuffer({.device = device}){};
};

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly

#endif	// BUFFERS_H
