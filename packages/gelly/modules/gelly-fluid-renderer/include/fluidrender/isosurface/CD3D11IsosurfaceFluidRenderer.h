#ifndef CD3D11ISOSURFACEFLUIDRENDERER_H
#define CD3D11ISOSURFACEFLUIDRENDERER_H

#include <GellyFluidSim.h>
#include <GellyInterfaceRef.h>

// Pretty much going to to be the same as the splatting textures, so we just
// reuse them.
#include "fluidrender/CD3D11ManagedBuffer.h"
#include "fluidrender/IFluidRenderer.h"
#include "fluidrender/IManagedBufferLayout.h"
#include "fluidrender/splatting/CD3D11SplattingFluidTextures.h"
#include "fluidrender/util/Kernel.h"
#include "renderdoc_app.h"

/**
 * \brief A fluid renderer based on the splatting + raymarching method
 * introduced in Tong Wu et al's "A Real-Time Adaptive Ray Marching Method for
 * Particle-Based Fluid Surface Reconstruction."
 * \note The splatting in this renderer is a highly specific and optimized
 * version of the original main splatting renderer.
 */
class CD3D11IsosurfaceFluidRenderer : public IFluidRenderer {
private:
	GellyInterfaceVal<IRenderContext> m_context = nullptr;
	GellyInterfaceVal<ISimData> m_simData = nullptr;
	CD3D11SplattingFluidTextures m_outputTextures;

	FluidRenderSettings m_settings;
	FluidRenderParams m_perFrameData{};

	struct {
		/**
		 * \brief Voxelizes the fluid domain into a 3D texture.
		 * Each voxel contains the number of particles in that voxel.
		 * This populates the particles in voxel buffer and the particle count
		 * buffer
		 */
		util::Kernel voxelize;
		/**
		 * \brief Constructs the BDG
		 */
		util::Kernel constructBDG;
		/**
		 * \brief Adaptively raymarches the domain using Tong wu et al's
		 * adaptive stepping method. Basically, it's sort of like a normal
		 * full-screen raymarch, but it uses adaptive tiles.
		 */
		util::Kernel raymarch;
	} m_kernels;

	struct {
		GellyInterfaceVal<IManagedBuffer> particleCount;
		/**
		 * \brief This is a strided buffer,
		 * where each member is a pointer to a particle index in the positions buffer.
		 * This is strided according to the maximum amount of particles in a voxel, set in the settings.
		 *
		 * \code{.hlsl}
		 * g_ParticlesInVoxels[voxelIndex * maxParticlesPerVoxel + wantedParticleIndex]
		 * \endcode
		 */
		GellyInterfaceVal<IManagedBuffer> particlesInVoxels;
		/**
		 * BDG is a 3D texture where each voxel contains a
		 * R16G16_FLOAT.
		 *
		 * R - p^max_c (max density for a given voxel)
		 *
		 * G - M_c (0 if no isosurface, 1 if isosurface) (M_c = p^max_c > sigma)
		 * (sigma is the threshold)
		 *
		 * \note It stands for binary density grid.
		 */
		GellyInterfaceVal<IManagedBuffer> bdg;
		GellyInterfaceVal<IManagedBuffer> positions;
		GellyInterfaceVal<IManagedBufferLayout> positionsLayout;
	} m_buffers = {};

	struct {
		GellyInterfaceVal<IManagedTexture> depth;
		GellyInterfaceVal<IManagedTexture> normal;
	} m_textures = {};

#ifdef _DEBUG
	RENDERDOC_API_1_1_2 *m_renderDoc = nullptr;
#endif

	void CreateBuffers();
	void CreateTextures();
	void CreateKernels();

	void ConstructMarchingBuffers();
	void Raymarch();

public:
	CD3D11IsosurfaceFluidRenderer();
	~CD3D11IsosurfaceFluidRenderer() override = default;

	void SetSimData(GellyObserverPtr<ISimData> simData) override;
	void AttachToContext(GellyObserverPtr<IRenderContext> context) override;
	GellyObserverPtr<IFluidTextures> GetFluidTextures() override;

	void Render() override;
	void EnableLowBitMode() override;

	void SetSettings(const Gelly::FluidRenderSettings &settings) override;
	void SetPerFrameParams(const Gelly::FluidRenderParams &params) override;

	// for now, these are null--they do nothing
	void PullPerParticleData() override;
	void SetPerParticleAbsorption(uint particleIndex, const float absorption[3])
		override;
	void PushPerParticleData() override;

	bool CheckFeatureSupport(GELLY_FEATURE feature) override;
};

#endif	// CD3D11ISOSURFACEFLUIDRENDERER_H
