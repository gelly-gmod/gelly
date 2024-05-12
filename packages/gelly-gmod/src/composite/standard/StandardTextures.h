#ifndef STANDARDTEXTURES_H
#define STANDARDTEXTURES_H
#include <d3d9.h>
#include <wrl/client.h>

#include "GellyInterfaceRef.h"
#include "composite/GellyResources.h"
#include "composite/UnownedResources.h"
#include "fluidrender/IManagedTexture.h"

using namespace Microsoft::WRL;

/**
 * Textures for the standard pipeline
 */
class StandardTextures {
private:
	GellyResources gellyResources;
	UnownedResources gmodResources;

	[[nodiscard]] std::pair<uint16_t, uint16_t> GetRenderContextSize() const;

	std::pair<ComPtr<IDirect3DTexture9>, GellyInterfaceVal<IManagedTexture>>
	CreateTexture(const char *name, D3DFORMAT format) const;

	void CreateFeatureTextures();
	void LinkFeatureTextures() const;

public:
	struct {
		GellyInterfaceVal<IManagedTexture> albedo;
		GellyInterfaceVal<IManagedTexture> normal;
		GellyInterfaceVal<IManagedTexture> depth;
		GellyInterfaceVal<IManagedTexture> position;
		GellyInterfaceVal<IManagedTexture> thickness;
	} gellyTextures{};

	struct {
		ComPtr<IDirect3DTexture9> albedo;
		ComPtr<IDirect3DTexture9> normal;
		ComPtr<IDirect3DTexture9> depth;
		ComPtr<IDirect3DTexture9> position;
		ComPtr<IDirect3DTexture9> thickness;
	} gmodTextures;

	StandardTextures(const GellyResources &gelly, const UnownedResources &gmod);
};

#endif	// STANDARDTEXTURES_H
