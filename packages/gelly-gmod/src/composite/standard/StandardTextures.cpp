#include "StandardTextures.h"

#include <utility>

std::pair<ComPtr<IDirect3DTexture9>, HANDLE> StandardTextures::CreateTexture(
	const char *name, D3DFORMAT format, int levels, float scale
) const {
	HANDLE sharedHandle = nullptr;
	ComPtr<IDirect3DTexture9> gmodTexture;

	const auto result = gmodResources.device->CreateTexture(
		width * scale,
		height * scale,
		levels,
		D3DUSAGE_RENDERTARGET,
		format,
		D3DPOOL_DEFAULT,
		gmodTexture.GetAddressOf(),
		&sharedHandle
	);

	if (FAILED(result)) {
		auto errorMessage =
			std::string("Failed to create texture: \"") + name + "\", reason: ";

		switch (result) {
			case D3DERR_INVALIDCALL:
				errorMessage += "D3DERR_INVALIDCALL";
			case D3DERR_OUTOFVIDEOMEMORY:
				errorMessage += "D3DERR_OUTOFVIDEOMEMORY";
			case E_OUTOFMEMORY:
				errorMessage += "E_OUTOFMEMORY";
			default:
				errorMessage += "Unknown error";
		}

		throw std::runtime_error(errorMessage);
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
		CreateTexture("gelly-gmod/albedo", D3DFMT_A16B16G16R16F, 1, 0.25f);

	std::tie(gmodTextures.normal, sharedHandles.normals) =
		CreateTexture("gelly-gmod/normal", D3DFMT_A16B16G16R16F);

	std::tie(gmodTextures.depth, sharedHandles.ellipsoidDepth) =
		CreateTexture("gelly-gmod/depth", D3DFMT_A32B32G32R32F);

	std::tie(gmodTextures.position, sharedHandles.positions) =
		CreateTexture("gelly-gmod/position", D3DFMT_A16B16G16R16F);

	std::tie(gmodTextures.thickness, sharedHandles.thickness) =
		CreateTexture("gelly-gmod/thickness", D3DFMT_A16B16G16R16F, 1, 0.25f);
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