#include "fluidrender/isosurface/CD3D11IsosurfaceFluidRenderer.h"

#include "ConstructBDGCS.h"
#include "RaymarchCS.h"
#include "VoxelizeCS.h"
#include "fluidrender/util/Exceptions.h"

CD3D11IsosurfaceFluidRenderer::CD3D11IsosurfaceFluidRenderer()
	: m_kernels({}), m_settings({}) {}

void CD3D11IsosurfaceFluidRenderer::CreateBuffers() {
	THROW_IF_FALSY(
		m_simData,
		"Renderer must be linked to simulation data before creating buffers"
	);

	THROW_IF_FALSY(m_context, "Renderer must be attached to a context");

	const uint maxParticles = m_simData->GetMaxParticles();
	const uint totalVoxels = m_settings.special.isosurface.domainWidth *
							 m_settings.special.isosurface.domainHeight *
							 m_settings.special.isosurface.domainDepth;
	BufferDesc particlePositionDesc = {};
	particlePositionDesc.format = BufferFormat::R32G32B32A32_FLOAT;
	particlePositionDesc.stride = sizeof(SimFloat4);
	particlePositionDesc.byteWidth = maxParticles * sizeof(SimFloat4);
	particlePositionDesc.type = BufferType::SHADER_RESOURCE;

	m_buffers.positions = m_context->CreateBuffer(particlePositionDesc);

	BufferDesc particleCountDesc = {};
	// Realistically there's never going to be > 65535 particles
	particleCountDesc.format = BufferFormat::R16_UINT;
	particleCountDesc.stride = sizeof(uint16_t);
	particleCountDesc.byteWidth = sizeof(uint16_t) * totalVoxels;
	particleCountDesc.type = BufferType::UNORDERED_ACCESS;

	m_buffers.particleCount = m_context->CreateBuffer(particleCountDesc);

	BufferDesc particlesInVoxelsDesc = {};
	particlesInVoxelsDesc.format = BufferFormat::R32_UINT;
	particlesInVoxelsDesc.stride = sizeof(uint32_t);
	particlesInVoxelsDesc.byteWidth =
		sizeof(uint32_t) * totalVoxels *
		m_settings.special.isosurface.maxParticlesInVoxel;
	particlesInVoxelsDesc.type = BufferType::UNORDERED_ACCESS;

	m_buffers.particlesInVoxels =
		m_context->CreateBuffer(particlesInVoxelsDesc);

	BufferDesc bdgDesc = {};
	bdgDesc.format = BufferFormat::R16G16_FLOAT;
	bdgDesc.stride = 4;	 // It's not pretty, but since we're using R16G16_FLOAT,
						 // we need to set the stride to 4 since a half float
						 // takes up 2 bytes and there's 2 of them.
	bdgDesc.byteWidth = totalVoxels * 4;
	bdgDesc.type = BufferType::UNORDERED_ACCESS;

	m_buffers.bdg = m_context->CreateBuffer(bdgDesc);

	// position layout is unused for now (comes later on when we need to perform
	// splatting)
}
