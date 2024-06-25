#ifndef STANDARDTEXTURES_H
#define STANDARDTEXTURES_H
#include <d3d9.h>
#include <wrl/client.h>

#include "GellyInterfaceRef.h"
#include "composite/GellyResources.h"
#include "composite/UnownedResources.h"
#include "fluidrender/IManagedTexture.h"

using namespace Microsoft::WRL;
using gelly::renderer::splatting::InputSharedHandles;

/**
 * Textures for the standard pipeline
 */
class StandardTextures {
private:
	UnownedResources gmodResources;
	InputSharedHandles sharedHandles;
	unsigned int width;
	unsigned int height;

	std::pair<ComPtr<IDirect3DTexture9>, HANDLE> CreateTexture(
		const char *name, D3DFORMAT format
	) const;

	void CreateFeatureTextures();

public:
	struct {
		ComPtr<IDirect3DTexture9> albedo;
		ComPtr<IDirect3DTexture9> normal;
		ComPtr<IDirect3DTexture9> depth;
		ComPtr<IDirect3DTexture9> position;
		ComPtr<IDirect3DTexture9> thickness;
		ComPtr<IDirect3DTexture9> foam;
	} gmodTextures;

	StandardTextures(
		const UnownedResources &gmod, unsigned int width, unsigned int height
	);
	~StandardTextures() = default;

	InputSharedHandles GetSharedHandles() const;
};

#endif	// STANDARDTEXTURES_H
