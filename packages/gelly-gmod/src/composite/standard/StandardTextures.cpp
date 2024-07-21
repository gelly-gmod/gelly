#include "StandardTextures.h"

#include <utility>

std::pair<ComPtr<IDirect3DTexture9>, HANDLE> StandardTextures::CreateTexture(
	const char *name, D3DFORMAT format, int levels
) const {
	HANDLE sharedHandle = nullptr;
	ComPtr<IDirect3DTexture9> gmodTexture;

	if (FAILED(gmodResources.device->CreateTexture(
			width,
			height,
			levels,
			D3DUSAGE_RENDERTARGET,
			format,
			D3DPOOL_DEFAULT,
			gmodTexture.GetAddressOf(),
			&sharedHandle
		))) {
		throw std::runtime_error(
			"Failed to create GMod texture '" + std::string(name) + "')"
		);
	}

	return {gmodTexture, sharedHandle};
}

void StandardTextures::CreateFeatureTextures() {
	/*
	std::tie(gmodTextures.albedo, gellyTextures.albedo) =
		CreateTexture("gelly-gmod/albedo", D3DFMT_A16B16G16R16F);

	std::tie(gmodTextures.normal, gellyTextures.normal) =
		CreateTexture("gelly-gmod/normal", D3DFMT_A16B16G16R16F);

	std::tie(gmodTextures.depth, gellyTextures.depth) =
		CreateTexture("gelly-gmod/depth", D3DFMT_A32B32G32R32F);

	std::tie(gmodTextures.position, gellyTextures.position) =
		CreateTexture("gelly-gmod/position", D3DFMT_A32B32G32R32F);

	std::tie(gmodTextures.thickness, gellyTextures.thickness) =
		CreateTexture("gelly-gmod/thickness", D3DFMT_A16B16G16R16F);

	std::tie(gmodTextures.foam, gellyTextures.foam) =
		CreateTexture("gelly-gmod/foam", D3DFMT_A16B16G16R16F);
		*/

	std::tie(gmodTextures.albedo, sharedHandles.albedo) =
		CreateTexture("gelly-gmod/albedo", D3DFMT_A16B16G16R16F);

	std::tie(gmodTextures.normal, sharedHandles.normals) =
		CreateTexture("gelly-gmod/normal", D3DFMT_A16B16G16R16F);

	std::tie(gmodTextures.depth, sharedHandles.ellipsoidDepth) =
		CreateTexture("gelly-gmod/depth", D3DFMT_A32B32G32R32F);

	std::tie(gmodTextures.position, sharedHandles.positions) =
		CreateTexture("gelly-gmod/position", D3DFMT_A32B32G32R32F);

	std::tie(gmodTextures.thickness, sharedHandles.thickness) =
		CreateTexture("gelly-gmod/thickness", D3DFMT_A16B16G16R16F);

	std::tie(gmodTextures.foam, sharedHandles.foam) =
		CreateTexture("gelly-gmod/foam", D3DFMT_A16B16G16R16F);
}

StandardTextures::StandardTextures(
	const UnownedResources &gmod, unsigned int width, unsigned int height
) :
	gmodResources(gmod), width(width), height(height) {
	CreateFeatureTextures();
}

gelly::renderer::splatting::InputSharedHandles
StandardTextures::GetSharedHandles() const {
	return sharedHandles;
}