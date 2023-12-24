#include "detail/d3d9/Texture.h"

#include <cassert>

#include "ErrorHandling.h"

#ifdef _DEBUG
#include <cassert>
#endif

using namespace d3d9;

Texture::Texture(
	IDirect3DTexture9 *texture,
	HANDLE sharedHandle,
	int width,
	int height,
	D3DFORMAT format
)
	: texture(texture),
	  sharedHandle(sharedHandle),
	  width(width),
	  height(height),
	  format(format) {}

Texture::Texture(
	IDirect3DDevice9 *device,
	int width,
	int height,
	D3DFORMAT format,
	DWORD usage
)
	: texture(nullptr),
	  sharedHandle(nullptr),
	  width(width),
	  height(height),
	  format(format) {
	DX("Failed to create texture",
	   device->CreateTexture(
		   width,
		   height,
		   1,
		   usage,
		   format,
		   D3DPOOL_DEFAULT,
		   texture.GetAddressOf(),
		   nullptr
	   ));
}

void Texture::SetupStage(int stage, IDirect3DDevice9 *device) const {
	DX("Failed to set texture", device->SetTexture(stage, texture.Get()));
}

void Texture::SetupSampler(int sampler, IDirect3DDevice9 *device) {
	DX("Failed to set sampler state",
	   device->SetSamplerState(sampler, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
	DX("Failed to set sampler state",
	   device->SetSamplerState(sampler, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));
	DX("Failed to set sampler state",
	   device->SetSamplerState(sampler, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP));
	DX("Failed to set sampler state",
	   device->SetSamplerState(sampler, D3DSAMP_MAGFILTER, D3DTEXF_POINT));
	DX("Failed to set sampler state",
	   device->SetSamplerState(sampler, D3DSAMP_MINFILTER, D3DTEXF_POINT));
	DX("Failed to set sampler state",
	   device->SetSamplerState(sampler, D3DSAMP_MIPFILTER, D3DTEXF_POINT));
}

void Texture::SetupAtStage(int stage, int sampler, IDirect3DDevice9 *device)
	const {
	SetupStage(stage, device);
	SetupSampler(sampler, device);
}

void Texture::DuplicateSurface(
	IDirect3DDevice9 *device, IDirect3DSurface9 *surface
) {
	IDirect3DSurface9 *textureSurface = nullptr;
	DX("Failed to get surface level",
	   texture->GetSurfaceLevel(0, &textureSurface));

#ifdef _DEBUG
	// Verify that the surface will actually be able to be copied.
	D3DSURFACE_DESC surfaceDesc;
	DX("Failed to get surface description", surface->GetDesc(&surfaceDesc));

	assert(surfaceDesc.Width == width);
	assert(surfaceDesc.Height == height);
	assert(surfaceDesc.Format == format);
#endif

	DX("Failed to copy surface",
	   device->StretchRect(
		   surface, nullptr, textureSurface, nullptr, D3DTEXF_NONE
	   ));
}

IDirect3DTexture9 *Texture::Get() const { return texture.Get(); }