#ifndef GELLY_IFLUIDRENDERER_H
#define GELLY_IFLUIDRENDERER_H

#include <GellyFluidSim.h>
#include <GellyInterface.h>
#include <GellyObserverPtr.h>

#include "IFluidTextures.h"
#include "IRenderContext.h"

namespace Gelly {
struct FluidRenderSettings {
	float particleRadius = 0.1f;
	int filterIterations = 72;
	int thicknessFilterIterations = 13;

	union {
		struct {
			float voxelSize = 0.04f;
			uint domainWidth = 128;
			uint domainHeight = 128;
			uint domainDepth = 128;
			/**
			 * \brief More accurate as it increases, but way slower.
			 * If your particles are pretty spread out (in FleX, this is a high
			 * rest distance), then around four or so is ok. If they're closer,
			 * consider increasing this.
			 */
			uint maxParticlesInVoxel = 32;
		} isosurface;
	} special = {};
};

/**
 * \brief This struct is shared between the GPU and the CPU for host
 * performance, so it always must be 16-byte aligned.
 * Some members will be filled out by the renderer, check the member
 * documentation for more information.
 */

struct FluidRenderParams {
	XMFLOAT4X4 view;
	XMFLOAT4X4 proj;
	XMFLOAT4X4 invView;
	XMFLOAT4X4 invProj;

	/**
	 * \note Automatically filled out by the renderer.
	 */
	float width;
	/**
	 * \note Automatically filled out by the renderer.
	 */
	float height;
	float thresholdRatio;
	float particleRadius;

	float nearPlane;
	float farPlane;
	XMFLOAT3 cameraPos;
	float pad[3] = {};
};
}  // namespace Gelly

gelly_interface IFluidRenderer : public IFeatureQuery {
public:
	virtual ~IFluidRenderer() = default;

	virtual void SetSimData(GellyObserverPtr<ISimData> simData) = 0;
	virtual void AttachToContext(GellyObserverPtr<IRenderContext> context) = 0;
	/**
	 * Gets the fluid textures used by this specific renderer.
	 * The end user can use this to set the textures for the renderer.
	 * @return
	 */
	virtual GellyObserverPtr<IFluidTextures> GetFluidTextures() = 0;
	virtual void Render() = 0;

	/**
	 * \brief Requests for the renderer to encode the last Render() call's depth
	 * into the low-bit support format.
	 * This format transforms the depth into the following format:
	 * where S is the cutoff threshold, which is 0.9 for now but may change.
	 * R: Eye depth, untouched and likely imprecise
	 * G: 0-S of the projected depth.
	 * B: S-1 of the projected depth.
	 */
	virtual void EnableLowBitMode() = 0;

	virtual void SetSettings(const FluidRenderSettings &settings) = 0;
	virtual void SetPerFrameParams(const FluidRenderParams &params) = 0;

	virtual void PullPerParticleData() = 0;
	/**
	 * \brief Sets the absorption of the given particle, will throw if particle
	 * data isn't pulled. \param particleIndex Index of the particle. \param
	 * absorption Absorption vector, in the format of RGB.
	 */
	virtual void SetPerParticleAbsorption(
		uint particleIndex, const float absorption[3]
	) = 0;
	virtual void PushPerParticleData() = 0;

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	/**
	 * \brief Uses RenderDoc to capture the fluid renderer's output. Normally,
	 * RenderDoc will only capture the output of the user's application. \note
	 * The API may change each time RenderDoc is updated, so this function may
	 * return false if the RenderDoc being used is incompatible.
	 */
	virtual bool EnableRenderDocCaptures() = 0;
#endif
};

#endif	// GELLY_IFLUIDRENDERER_H
