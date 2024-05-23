#ifndef GELLY_GELLYFLUIDRENDER_H
#define GELLY_GELLYFLUIDRENDER_H
#include "fluidrender/IFluidRenderer.h"
#include "fluidrender/IRenderContext.h"

namespace Gelly {
IRenderContext *CreateD3D11FluidRenderContext(uint16_t width, uint16_t height);

IFluidRenderer *CreateD3D11IsosurfaceFluidRenderer(IRenderContext *context);
IFluidRenderer *CreateD3D11SplattingFluidRenderer(IRenderContext *context);

void DestroyGellyFluidRenderContext(IRenderContext *context);
void DestroyGellyFluidRenderer(IFluidRenderer *renderer);
}  // namespace Gelly

#endif	// GELLY_GELLYFLUIDRENDER_H