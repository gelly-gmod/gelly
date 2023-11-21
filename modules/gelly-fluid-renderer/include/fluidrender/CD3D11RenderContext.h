#ifndef GELLY_D3D11RENDERCONTEXT_H
#define GELLY_D3D11RENDERCONTEXT_H

#include <d3d11.h>

#include <string>
#include <unordered_map>

#include "CD3D11ManagedTexture.h"
#include "GellyObserverPtr.h"
#include "IRenderContext.h"

class CD3D11RenderContext : public IRenderContext {
private:
	ID3D11Device *device;
	ID3D11DeviceContext *deviceContext;

	std::unordered_map<std::string, IManagedTexture *> textures;
	std::vector<IManagedShader *> shaders{};

	uint16_t width;
	uint16_t height;

	void CreateDeviceAndContext();
	void CreateAllShaders();
	void DestroyAllShaders();
	void CreateAllTextures();
	void DestroyAllTextures();
	void ReleaseDevice();

public:
	CD3D11RenderContext(uint16_t width, uint16_t height);
	~CD3D11RenderContext() override;

	void *GetRenderAPIResource(RenderAPIResource resource) override;
	ContextRenderAPI GetRenderAPI() override;

	GellyObserverPtr<IManagedTexture> CreateTexture(
		const char *name, const TextureDesc &desc
	) override;

	GellyObserverPtr<IManagedTexture> CreateSharedTexture(
		const char *name, HANDLE sharedHandle
	) override;

	GellyObserverPtr<IManagedShader> CreateShader(
		const uint8_t *bytecode, size_t bytecodeSize, ShaderType type
	) override;

	void DestroyTexture(const char *name) override;

	void SetDimensions(uint16_t width, uint16_t height) override;
	void GetDimensions(uint16_t &width, uint16_t &height) override;

	void SubmitWork() override;
};

#endif	// GELLY_D3D11RENDERCONTEXT_H
