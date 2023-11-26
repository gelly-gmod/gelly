#ifndef CD3D11MANAGEDDEPTHBUFFER_H
#define CD3D11MANAGEDDEPTHBUFFER_H

#include <d3d11.h>

#include "IManagedDepthBuffer.h"

class CD3D11ManagedDepthBuffer : public IManagedDepthBuffer {
private:
	ID3D11DepthStencilState *depthStencilState;
	ID3D11DepthStencilView *depthStencilView;
	ID3D11Texture2D *depthStencilTexture;

	GellyInterfaceVal<IRenderContext> context;
	Gelly::DepthBufferDesc desc;

public:
	CD3D11ManagedDepthBuffer();
	~CD3D11ManagedDepthBuffer() override;

	void SetDesc(const Gelly::DepthBufferDesc &desc) override;
	Gelly::DepthBufferDesc GetDesc() const override;

	void AttachToContext(GellyInterfaceVal<IRenderContext> context) override;
	void Clear(float depth) override;

	void Create() override;
	void Destroy() override;

	void *RequestResource(Gelly::DepthBufferResource resource) override;
};

#endif	// CD3D11MANAGEDDEPTHBUFFER_H
