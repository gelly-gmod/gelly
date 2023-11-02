#ifndef GELLY_IFLUIDRENDERER_H
#define GELLY_IFLUIDRENDERER_H

#include <GellyFluidSim.h>
#include <GellyInterface.h>
#include <GellyObserverPtr.h>

#include "IRenderContext.h"

namespace Gelly {
struct FluidRenderSettings {
	float particleRadius = 0.1f;
};

}  // namespace Gelly

gelly_interface IFluidRenderer {
public:
	virtual ~IFluidRenderer() = default;

	virtual void SetSimData(GellyObserverPtr<ISimData> simData) = 0;
	virtual void AttachToContext(GellyObserverPtr<IRenderContext> context) = 0;
	virtual void Render() = 0;

	virtual void SetSettings(const Gelly::FluidRenderSettings &settings) = 0;
};

#endif	// GELLY_IFLUIDRENDERER_H
