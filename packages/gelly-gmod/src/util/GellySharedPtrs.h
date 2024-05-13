#ifndef GELLYSHAREDPTRS_H
#define GELLYSHAREDPTRS_H

#include <memory>

#include "GellyFluidRender.h"
#include "fluidrender/IFluidRenderer.h"

template <class... Args>
std::shared_ptr<IFluidRenderer> MakeFluidRenderer(Args &&...args) {
	return std::shared_ptr<IFluidRenderer>(
		CreateD3D11SplattingFluidRenderer(std::forward<Args>(args)...),
		[](auto *ptr) { DestroyGellyFluidRenderer(ptr); }
	);
}

template <class... Args>
std::shared_ptr<IRenderContext> MakeRenderContext(Args &&...args) {
	return std::shared_ptr<IRenderContext>(
		CreateD3D11FluidRenderContext(std::forward<Args>(args)...),
		[](auto *ptr) { DestroyGellyRenderContext(ptr); }
	);
}

template <class... Args>
std::shared_ptr<ISimContext> MakeSimContext(Args &&...args) {
	return std::shared_ptr<ISimContext>(
		CreateD3D11SimContext(std::forward<Args>(args)...),
		[](auto *ptr) { DestroyGellySimContext(ptr); }
	);
}

template <class... Args>
std::shared_ptr<IFluidSimulation> MakeFluidSimulation(Args &&...args) {
	return std::shared_ptr<IFluidSimulation>(
		CreateD3D11FlexFluidSimulation(std::forward<Args>(args)...),
		[](auto *ptr) { DestroyGellyFluidSimulation(ptr); }
	);
}

#endif	// GELLYSHAREDPTRS_H
