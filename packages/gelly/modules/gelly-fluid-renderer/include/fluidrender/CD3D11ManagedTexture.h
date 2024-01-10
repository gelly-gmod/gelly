#ifndef GELLY_CD3D11MANAGEDTEXTURE_H
#define GELLY_CD3D11MANAGEDTEXTURE_H

#include <d3d11.h>

#include "IManagedTexture.h"
#include "IRenderContext.h"

class CD3D11ManagedTexture : public IManagedTexture {
private:
	IRenderContext *context;
	TextureDesc desc;

	// don't think this warrants anything more complicated, if it's 3d then only
	// texture3D is used- vice versa
	ID3D11Texture2D *texture2D = nullptr;
	ID3D11Texture3D *texture3D = nullptr;

	ID3D11ShaderResourceView *srv;
	ID3D11RenderTargetView *rtv;
	ID3D11UnorderedAccessView *uav;
	ID3D11SamplerState *sampler;

	ID3D11Resource *d3d11Resource;

	inline bool Is3D() const { return desc.depth > 1; }

public:
	CD3D11ManagedTexture();
	~CD3D11ManagedTexture() override;

	void SetDesc(const TextureDesc &desc) override;
	[[nodiscard]] const TextureDesc &GetDesc() const override;
	void Create3DTexture(ID3D11Device *device, DXGI_FORMAT format);
	void Create2DTexture(ID3D11Device *device, DXGI_FORMAT format);

	bool Create() override;
	void Destroy() override;

	void AttachToContext(IRenderContext *context) override;
	GellyObserverPtr<IRenderContext> GetParentContext() override;

	void SetFullscreenSize() override;

	void *GetSharedHandle() override;
	void *GetResource(TextureResource resource) override;

	void BindToPipeline(
		TextureBindStage stage, uint8_t slot, OptionalDepthBuffer depthBuffer
	) override;

	void Clear(const float color[4]) override;

	void CopyToTexture(GellyInterfaceRef<IManagedTexture> texture) override;
};

#endif	// GELLY_CD3D11MANAGEDTEXTURE_H
