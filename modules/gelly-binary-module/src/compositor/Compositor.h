#ifndef GELLY_COMPOSITOR_H
#define GELLY_COMPOSITOR_H

#include <GellyRenderer.h>
#include <d3d9.h>
#include <wrl.h>

#include "passes/Composite.h"
#include "passes/Pass.h"

using namespace Microsoft::WRL;

class Compositor {
	IDirect3DDevice9Ex *device;
	Composite compositePass;

	ComPtr<IDirect3DVertexBuffer9> screenQuad;
	// no declaration needed, we use FVF instead

	struct NDCVertex {
		// It's not in viewport space, but in NDC space.

		static const DWORD FVF = D3DFVF_XYZW | D3DFVF_TEX1;
		float x, y, z, w;
		float u, v;
	};

	ComPtr<IDirect3DVertexShader9> vertexShader;
	ComPtr<IDirect3DPixelShader9> pixelShader;

	// We're not using a ComPtr here because this is meant to be owned
	// by Gelly. There should be nothing happening with the ref count.
	SharedTextures gbuffer;

	// We have to make sure to look like we were never here to the rest of the
	// game, so we store every single previous value of any function we called.

	struct {
		_D3DTEXTUREADDRESS addressU1;
		_D3DTEXTUREADDRESS addressV1;
		_D3DTEXTUREFILTERTYPE magFilter1;
		_D3DTEXTUREFILTERTYPE minFilter1;
		_D3DTEXTUREFILTERTYPE mipFilter1;

		_D3DTEXTUREADDRESS addressU2;
		_D3DTEXTUREADDRESS addressV2;
		_D3DTEXTUREFILTERTYPE magFilter2;
		_D3DTEXTUREFILTERTYPE minFilter2;
		_D3DTEXTUREFILTERTYPE mipFilter2;

		IDirect3DVertexShader9 *vertexShader;
		IDirect3DPixelShader9 *pixelShader;
		IDirect3DVertexBuffer9 *streamSource;
		IDirect3DBaseTexture9 *texture0;
		IDirect3DBaseTexture9 *texture1;
		UINT streamOffset;
		UINT streamStride;
		DWORD fvf;
		DWORD lighting;
		DWORD ztest;
		DWORD alphaBlend;

		float constant0[4];
	} previous{};

	void CreateScreenQuad();
	void CreateShaders();
	void SaveState();
	void RestorePreviousState();

public:
	struct {
		float zValue = 0.f;
	} debugConstants{};

	explicit Compositor(IDirect3DDevice9Ex *device, SharedTextures *gbuffer);
	~Compositor() = default;
	void Composite();
};

#endif	// GELLY_COMPOSITOR_H
