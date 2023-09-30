#include "Compositor.h"

static PassGBuffer CreateGBuffer(
	IDirect3DDevice9Ex *device,
	int width,
	int height,
	SharedTextures *sharedTextures
) {
	auto format = D3DFMT_A8R8G8B8;
	auto usage = D3DUSAGE_RENDERTARGET;

	return {
		.framebuffer = d3d9::Texture(device, width, height, format, usage),
		.shared = *sharedTextures,
	};
}

Compositor::Compositor(
	IDirect3DDevice9Ex *device,
	int width,
	int height,
	SharedTextures *sharedTextures
)
	: device(device),
	  gbuffer(CreateGBuffer(device, width, height, sharedTextures)),
	  compositePass(device){};

void Compositor::SaveState() {
	// Copy all the original values into our previous buffer.
	device->GetStreamSource(
		0,
		&previous.streamSource,
		&previous.streamOffset,
		&previous.streamStride
	);
	device->GetFVF(&previous.fvf);
	device->GetVertexShader(&previous.vertexShader);
	device->GetPixelShader(&previous.pixelShader);
	device->GetSamplerState(
		0, D3DSAMP_ADDRESSU, reinterpret_cast<DWORD *>(&previous.addressU1)
	);
	device->GetSamplerState(
		0, D3DSAMP_ADDRESSV, reinterpret_cast<DWORD *>(&previous.addressV1)
	);
	device->GetSamplerState(
		0, D3DSAMP_MAGFILTER, reinterpret_cast<DWORD *>(&previous.magFilter1)
	);
	device->GetSamplerState(
		0, D3DSAMP_MINFILTER, reinterpret_cast<DWORD *>(&previous.minFilter1)
	);
	device->GetSamplerState(
		0, D3DSAMP_MIPFILTER, reinterpret_cast<DWORD *>(&previous.mipFilter1)
	);
	device->GetTexture(0, &previous.texture0);
	device->GetSamplerState(
		1, D3DSAMP_ADDRESSU, reinterpret_cast<DWORD *>(&previous.addressU2)
	);
	device->GetSamplerState(
		1, D3DSAMP_ADDRESSV, reinterpret_cast<DWORD *>(&previous.addressV2)
	);
	device->GetSamplerState(
		1, D3DSAMP_MAGFILTER, reinterpret_cast<DWORD *>(&previous.magFilter2)
	);
	device->GetSamplerState(
		1, D3DSAMP_MINFILTER, reinterpret_cast<DWORD *>(&previous.minFilter2)
	);
	device->GetSamplerState(
		1, D3DSAMP_MIPFILTER, reinterpret_cast<DWORD *>(&previous.mipFilter2)
	);
	device->GetTexture(1, &previous.texture1);

	device->GetRenderState(D3DRS_LIGHTING, &previous.lighting);
	device->GetRenderState(D3DRS_ZENABLE, &previous.ztest);
	device->GetRenderState(D3DRS_ALPHABLENDENABLE, &previous.alphaBlend);

	device->GetPixelShaderConstantF(0, previous.constant0, 1);
}

void Compositor::RestorePreviousState() {
	if (previous.streamSource) {
		DX("Failed to restore stream source",
		   device->SetStreamSource(
			   0,
			   previous.streamSource,
			   previous.streamOffset,
			   previous.streamStride
		   ));
	}

	if (previous.vertexShader) {
		DX("Failed to restore vertex shader",
		   device->SetVertexShader(previous.vertexShader));
	}

	if (previous.pixelShader) {
		DX("Failed to set pixel shader constants",
		   device->SetPixelShaderConstantF(0, previous.constant0, 1));

		DX("Failed to restore pixel shader",
		   device->SetPixelShader(previous.pixelShader));
	}

	if (previous.texture0) {
		DX("Failed to restore texture",
		   device->SetTexture(0, previous.texture0));
	}

	if (previous.texture1) {
		DX("Failed to restore texture",
		   device->SetTexture(1, previous.texture1));
	}

	DX("Failed to restore sampler state",
	   device->SetSamplerState(0, D3DSAMP_ADDRESSU, previous.addressU1));
	DX("Failed to restore sampler state",
	   device->SetSamplerState(0, D3DSAMP_ADDRESSV, previous.addressV1));
	DX("Failed to restore sampler state",
	   device->SetSamplerState(0, D3DSAMP_MAGFILTER, previous.magFilter1));
	DX("Failed to restore sampler state",
	   device->SetSamplerState(0, D3DSAMP_MINFILTER, previous.minFilter1));
	DX("Failed to restore sampler state",
	   device->SetSamplerState(0, D3DSAMP_MIPFILTER, previous.mipFilter1));

	DX("Failed to restore sampler state",
	   device->SetSamplerState(1, D3DSAMP_ADDRESSU, previous.addressU2));
	DX("Failed to restore sampler state",
	   device->SetSamplerState(1, D3DSAMP_ADDRESSV, previous.addressV2));
	DX("Failed to restore sampler state",
	   device->SetSamplerState(1, D3DSAMP_MAGFILTER, previous.magFilter2));
	DX("Failed to restore sampler state",
	   device->SetSamplerState(1, D3DSAMP_MINFILTER, previous.minFilter2));
	DX("Failed to restore sampler state",
	   device->SetSamplerState(1, D3DSAMP_MIPFILTER, previous.mipFilter2));

	DX("Failed to set render state",
	   device->SetRenderState(D3DRS_LIGHTING, previous.lighting));
	DX("Failed to set render state",
	   device->SetRenderState(D3DRS_ZENABLE, previous.ztest));
	DX("Failed to set render state",
	   device->SetRenderState(D3DRS_ALPHABLENDENABLE, previous.alphaBlend));
}

void Compositor::UpdateFramebufferCopy() {
	IDirect3DSurface9 *framebufferSurface = nullptr;
	DX("Failed to get backbuffer",
	   device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &framebufferSurface)
	);

	gbuffer.framebuffer.DuplicateSurface(device, framebufferSurface);
}

void Compositor::Composite() {
	SaveState();
	UpdateFramebufferCopy();
	
	PassResources resources{
		.device = device,
		.gbuffer = &gbuffer,
	};

	compositePass.Render(&resources);

	// Restore, giving back control to GMod.
	// This is essential because of technical limitations that force us to
	// arbitrarily set up our own rendering pipeline.
	RestorePreviousState();
}
