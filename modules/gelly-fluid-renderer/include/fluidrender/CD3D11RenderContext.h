#ifndef GELLY_D3D11RENDERCONTEXT_H
#define GELLY_D3D11RENDERCONTEXT_H

#include <d3d11.h>

#include <string>
#include <unordered_map>

#include "CD3D11ManagedTexture.h"
#include "IRenderContext.h"

class CD3D11RenderContext : public IRenderContext {
private:
	ID3D11Device *device;
	ID3D11DeviceContext *deviceContext;

	std::unordered_map<std::string, CD3D11ManagedTexture *> textures;

	uint16_t width;
	uint16_t height;

	void CreateDeviceAndContext();
	void CreateAllTextures();
	void DestroyAllTextures();

public:
	CD3D11RenderContext(uint16_t width, uint16_t height);
	~CD3D11RenderContext() override;

	void *GetRenderAPIResource(RenderAPIResource resource) override;
	ContextRenderAPI GetRenderAPI() override;

	IManagedTexture *CreateTexture(
		const char *name, const GellyTextureDesc &desc
	) override;

	void DestroyTexture(const char *name) override;

	void SetDimensions(uint16_t width, uint16_t height) override;
	void GetDimensions(uint16_t &width, uint16_t &height) override;
};

#endif	// GELLY_D3D11RENDERCONTEXT_H
