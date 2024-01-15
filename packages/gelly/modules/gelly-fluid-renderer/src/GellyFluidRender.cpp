#include "GellyFluidRender.h"

#include "fluidrender/CD3D11RenderContext.h"
#include "fluidrender/IRenderContext.h"
#include "fluidrender/isosurface/CD3D11IsosurfaceFluidRenderer.h"
#include "fluidrender/splatting/CD3D11SplattingFluidRenderer.h"

IRenderContext *Gelly::CreateD3D11FluidRenderContext(
	uint16_t width, uint16_t height
) {
	auto *context = new CD3D11RenderContext(width, height);
	return context;
}

IFluidRenderer *Gelly::CreateD3D11IsosurfaceFluidRenderer(
	IRenderContext *context
) {
	auto *renderer = new CD3D11IsosurfaceFluidRenderer();
	renderer->AttachToContext(context);
	return renderer;
}

IFluidRenderer *Gelly::CreateD3D11SplattingFluidRenderer(IRenderContext *context
) {
	auto *renderer = new CD3D11SplattingFluidRenderer();
	renderer->AttachToContext(context);
	return renderer;
}

void Gelly::DestroyGellyFluidRenderContext(IRenderContext *context) {
	delete context;
}

void Gelly::DestroyGellyFluidRenderer(IFluidRenderer *renderer) {
	delete renderer;
}