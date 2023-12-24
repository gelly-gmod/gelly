#ifndef GELLY_CD3D11DEBUGFLUIDTEXTURES_H
#define GELLY_CD3D11DEBUGFLUIDTEXTURES_H

#include "IFluidTextures.h"

class CD3D11DebugFluidTextures : public IFluidTextures {
private:
	GellyObserverPtr<IManagedTexture> albedo;
	GellyObserverPtr<IManagedTexture> normal;
	GellyObserverPtr<IManagedTexture> depth;
	GellyObserverPtr<IManagedTexture> positions;
	GellyObserverPtr<IManagedTexture> thickness;

public:
	CD3D11DebugFluidTextures();
	~CD3D11DebugFluidTextures() override = default;

	void SetFeatureTexture(
		FluidFeatureType feature, GellyObserverPtr<IManagedTexture> texture
	) override;

	GellyObserverPtr<IManagedTexture> GetFeatureTexture(FluidFeatureType feature
	) override;

	bool IsInitialized() override;
};

#endif	// GELLY_CD3D11DEBUGFLUIDTEXTURES_H
