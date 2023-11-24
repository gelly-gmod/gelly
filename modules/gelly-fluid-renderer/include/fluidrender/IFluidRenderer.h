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
};

struct FluidRenderParams {
	XMFLOAT4X4 view;
	XMFLOAT4X4 proj;
	XMFLOAT4X4 invView;
	XMFLOAT4X4 invProj;
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
};

#endif	// GELLY_IFLUIDRENDERER_H
