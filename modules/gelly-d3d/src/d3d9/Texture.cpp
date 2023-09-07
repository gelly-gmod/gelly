#include "detail/d3d9/Texture.h"

#include "ErrorHandling.h"

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

IDirect3DTexture9 *Texture::Get() const { return texture.Get(); }