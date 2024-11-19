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

ComPtr<IDirect3DTexture9> StandardTextures::CreateTextureUnshared(
	const char *name, D3DFORMAT format, int levels, float scale
) const {
	ComPtr<IDirect3DTexture9> gmodTexture;

	const auto result = gmodResources.device->CreateTexture(
		width * scale,
		height * scale,
		levels,
		D3DUSAGE_RENDERTARGET,
		format,
		D3DPOOL_DEFAULT,
		gmodTexture.GetAddressOf(),
		nullptr
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

	return gmodTexture;
}

void StandardTextures::CreateFeatureTextures(float scale) {
	// albedo and thickness are always fixed at quarter resolution
	std::tie(gmodTextures.albedo, sharedHandles.albedo) =
		CreateTexture("gelly-gmod/albedo", D3DFMT_A16B16G16R16F, 1, 0.5f);

	std::tie(gmodTextures.normal, sharedHandles.normals) =
		CreateTexture("gelly-gmod/normal", D3DFMT_A16B16G16R16F, 1, scale);

	std::tie(gmodTextures.depth, sharedHandles.ellipsoidDepth) =
		CreateTexture("gelly-gmod/depth", D3DFMT_A32B32G32R32F, 1, scale);

	std::tie(gmodTextures.thickness, sharedHandles.thickness) =
		CreateTexture("gelly-gmod/thickness", D3DFMT_A16B16G16R16F, 1, 0.5f);

	// final output doesn't really require precision, the backbuffer has 8 bits
	// per channel anyway
	gmodTextures.final = CreateTextureUnshared(
		"gelly-gmod/final", D3DFMT_A16B16G16R16F, 1, scale
	);
}

StandardTextures::StandardTextures(
	const UnownedResources &gmod,
	unsigned int width,
	unsigned int height,
	float scale
) :
	gmodResources(gmod), width(width), height(height) {
	CreateFeatureTextures(scale);
	gmodTextures.final->GetSurfaceLevel(0, finalSurface.GetAddressOf());
}

gelly::renderer::splatting::InputSharedHandles
StandardTextures::GetSharedHandles() const {
	return sharedHandles;
}