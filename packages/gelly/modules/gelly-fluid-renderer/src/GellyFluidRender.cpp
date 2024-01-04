#include "GellyFluidRender.h"

#include "..\include\fluidrender\CD3D11SplattingFluidRenderer.h"
#include "fluidrender/CD3D11RenderContext.h"
#include "fluidrender/IRenderContext.h"

IRenderContext *Gelly::CreateD3D11FluidRenderContext(
	uint16_t width, uint16_t height
) {
	auto *context = new CD3D11RenderContext(width, height);
	return context;
}

IFluidRenderer *Gelly::CreateD3D11SplattingFluidRenderer(IRenderContext *context
) {
	auto *renderer = new CD3D11SplattingFluidRenderer();
	renderer->AttachToContext(context);
	return renderer;
}