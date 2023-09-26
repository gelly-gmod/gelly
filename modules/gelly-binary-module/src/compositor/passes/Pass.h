#ifndef GELLY_PASS_H
#define GELLY_PASS_H

#include <GellyD3D.h>
#include <GellyRenderer.h>
#include <d3d9.h>
#include <wrl.h>

using namespace Microsoft::WRL;

struct PassResources {
	IDirect3DDevice9 *device;
	SharedTextures *gbuffer;
};

/**
 * A strictly screen-space rendering pass.
 * @note This is meant to be subclassed, but also contains helper methods for
 * rendering.
 */
class Pass {
private:
	ComPtr<IDirect3DVertexBuffer9> screenQuad;
	// This vertex is in NDC space.
	struct ScreenQuadVertex {
		static const DWORD FVF = D3DFVF_XYZW | D3DFVF_TEX1;
		float x, y, z, w;
		float u, v;
	};

	ComPtr<IDirect3DVertexShader9> vertexShader;
	ComPtr<IDirect3DPixelShader9> pixelShader;

	void CreateScreenQuad(IDirect3DDevice9 *device);
	void CreateShaders(IDirect3DDevice9 *device, const char *pixelShaderSource);

protected:
	void BindShadersAndQuad(IDirect3DDevice9 *device);
	void ExecutePass(IDirect3DDevice9 *device);

public:
	Pass(IDirect3DDevice9 *device, const char *pixelShaderSource);
	~Pass() = default;

	virtual void Render(PassResources *resources) = 0;
};

#endif	// GELLY_PASS_H
