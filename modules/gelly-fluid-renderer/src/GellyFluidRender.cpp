#include "GellyFluidRender.h"

#include "fluidrender/CD3D11DebugFluidRenderer.h"
#include "fluidrender/CD3D11RenderContext.h"
#include "fluidrender/IRenderContext.h"

IRenderContext *Gelly::CreateD3D11FluidRenderContext(
	uint16_t width, uint16_t height
) {
	auto *context = new CD3D11RenderContext(width, height);
	return context;
}

IFluidRenderer *Gelly::CreateD3D11DebugFluidRenderer(
	IRenderContext *context, const int maxParticles
) {
	auto *renderer = new CD3D11DebugFluidRenderer();
	renderer->SetMaxParticles(maxParticles);
	renderer->AttachToContext(context);
	return renderer;
}