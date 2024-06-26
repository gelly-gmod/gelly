#ifndef GELLYSHAREDPTRS_H
#define GELLYSHAREDPTRS_H

#include <memory>

template <class... Args>
std::shared_ptr<ISimContext> MakeSimContext(Args &&...args) {
	return std::shared_ptr<ISimContext>(
		CreateD3D11SimContext(std::forward<Args>(args)...),
		[](auto *ptr) { /* no-op till gelly fixes this */ }
	);
}

template <class... Args>
std::shared_ptr<IFluidSimulation> MakeFluidSimulation(Args &&...args) {
	return std::shared_ptr<IFluidSimulation>(
		CreateD3D11FlexFluidSimulation(std::forward<Args>(args)...),
		[](auto *ptr) { DestroyGellyFluidSim(ptr); }
	);
}

#endif	// GELLYSHAREDPTRS_H
