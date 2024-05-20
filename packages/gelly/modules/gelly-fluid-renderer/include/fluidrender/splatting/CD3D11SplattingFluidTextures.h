#ifndef GELLY_CD3D11DEBUGFLUIDTEXTURES_H
#define GELLY_CD3D11DEBUGFLUIDTEXTURES_H

#include "fluidrender/IFluidTextures.h"

class CD3D11SplattingFluidTextures : public IFluidTextures {
private:
	GellyObserverPtr<IManagedTexture> albedo;
	GellyObserverPtr<IManagedTexture> normal;
	GellyObserverPtr<IManagedTexture> depth;
	GellyObserverPtr<IManagedTexture> positions;
	GellyObserverPtr<IManagedTexture> thickness;
	GellyObserverPtr<IManagedTexture> foam;

public:
	CD3D11SplattingFluidTextures();
	~CD3D11SplattingFluidTextures() override = default;

	void SetFeatureTexture(
		FluidFeatureType feature, GellyObserverPtr<IManagedTexture> texture
	) override;

	GellyObserverPtr<IManagedTexture> GetFeatureTexture(FluidFeatureType feature
	) override;

	bool IsInitialized() override;
};

#endif	// GELLY_CD3D11DEBUGFLUIDTEXTURES_H
