#include "StandardTextures.h"

#include <utility>

#include "fluidrender/IRenderContext.h"

std::pair<uint16_t, uint16_t> StandardTextures::GetRenderContextSize() const {
	uint16_t width, height;
	gellyResources.context->GetDimensions(width, height);
	return std::make_pair(width, height);
}

std::pair<ComPtr<IDirect3DTexture9>, GellyInterfaceVal<IManagedTexture>>
StandardTextures::CreateTexture(const char *name, D3DFORMAT format) const {
	HANDLE sharedHandle = nullptr;
	ComPtr<IDirect3DTexture9> gmodTexture;

	auto [width, height] = GetRenderContextSize();

	if (FAILED(gmodResources.device->CreateTexture(
			width,
			height,
			1,
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

	GellyInterfaceVal<IManagedTexture> gellyTexture =
		gellyResources.context->CreateSharedTexture(
			name, sharedHandle, ContextRenderAPI::D3D9Ex
		);

	return std::make_pair(gmodTexture, gellyTexture);
}

void StandardTextures::CreateFeatureTextures() {
	std::tie(gmodTextures.albedo, gellyTextures.albedo) =
		CreateTexture("gelly-gmod/albedo", D3DFMT_A32B32G32R32F);

	std::tie(gmodTextures.normal, gellyTextures.normal) =
		CreateTexture("gelly-gmod/normal", D3DFMT_A32B32G32R32F);

	std::tie(gmodTextures.depth, gellyTextures.depth) =
		CreateTexture("gelly-gmod/depth", D3DFMT_A32B32G32R32F);

	std::tie(gmodTextures.position, gellyTextures.position) =
		CreateTexture("gelly-gmod/position", D3DFMT_A32B32G32R32F);

	std::tie(gmodTextures.thickness, gellyTextures.thickness) =
		CreateTexture("gelly-gmod/thickness", D3DFMT_A16B16G16R16F);
}

void StandardTextures::LinkFeatureTextures() const {
	gellyResources.textures->SetFeatureTexture(ALBEDO, gellyTextures.albedo);
	gellyResources.textures->SetFeatureTexture(NORMALS, gellyTextures.normal);
	gellyResources.textures->SetFeatureTexture(DEPTH, gellyTextures.depth);
	gellyResources.textures->SetFeatureTexture(
		POSITIONS, gellyTextures.position
	);
	gellyResources.textures->SetFeatureTexture(
		THICKNESS, gellyTextures.thickness
	);
}

StandardTextures::StandardTextures(
	const GellyResources &gelly, const UnownedResources &gmod
)
	: gellyResources(gelly), gmodResources(gmod) {
	CreateFeatureTextures();
	LinkFeatureTextures();
}
