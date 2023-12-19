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
	int filterIterations = 33;
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
};
}  // namespace Gelly

gelly_interface IFluidRenderer {
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

	virtual void SetSettings(const FluidRenderSettings &settings) = 0;
	virtual void SetPerFrameParams(const FluidRenderParams &params) = 0;

#ifdef _DEBUG
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
