#ifndef GELLY_CD3D11MANAGEDTEXTURE_H
#define GELLY_CD3D11MANAGEDTEXTURE_H

#include <d3d11.h>

#include "IManagedTexture.h"
#include "IRenderContext.h"

class CD3D11ManagedTexture : public IManagedTexture {
private:
	IRenderContext *context;
	TextureDesc desc;

	ID3D11Texture2D *texture;
	ID3D11ShaderResourceView *srv;
	ID3D11RenderTargetView *rtv;
	ID3D11UnorderedAccessView *uav;

public:
	CD3D11ManagedTexture();
	~CD3D11ManagedTexture() override;

	void SetDesc(const TextureDesc &desc) override;
	[[nodiscard]] const TextureDesc &GetDesc() const override;

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
};

#endif	// GELLY_CD3D11MANAGEDTEXTURE_H
