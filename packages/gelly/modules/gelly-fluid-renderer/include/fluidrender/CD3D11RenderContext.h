#ifndef GELLY_D3D11RENDERCONTEXT_H
#define GELLY_D3D11RENDERCONTEXT_H

#include <d3d11.h>
#include <d3d11_4.h>

#include <string>
#include <unordered_map>

#include "CD3D11ManagedTexture.h"
#include "GellyObserverPtr.h"
#include "IRenderContext.h"

class CD3D11RenderContext : public IRenderContext {
private:
	ID3D11Device *device;
	ID3D11Device5 *device5;

	ID3D11DeviceContext *deviceContext;
	ID3D11DeviceContext4 *deviceContext4;

#ifdef _DEBUG
	ID3D11Debug *debug = nullptr;
	ID3D11InfoQueue *infoQueue = nullptr;
#endif

	ID3D11RasterizerState *rasterizerState;
	RasterizerFlags rasterizerFlags = RasterizerFlags::NONE;

	std::unordered_map<std::string, IManagedTexture *> textures;
	std::vector<IManagedShader *> shaders{};

	ID3D11BlendState *blendState;

	ID3D11Fence *frameCompletionFence;

	uint16_t width;
	uint16_t height;

	bool overrideDimensions = false;
	uint16_t overrideWidth = 0;
	uint16_t overrideHeight = 0;

	uint32_t fenceValue = 0;
	HANDLE fenceEvent = nullptr;

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
		const char *name, HANDLE sharedHandle, ContextRenderAPI guestAPI
	) override;

	GellyObserverPtr<IManagedShader> CreateShader(
		const uint8_t *bytecode, size_t bytecodeSize, ShaderType type
	) override;

	GellyObserverPtr<IManagedBuffer> CreateBuffer(const BufferDesc &desc
	) override;

	GellyInterfaceVal<IManagedBufferLayout> CreateBufferLayout(
		const BufferLayoutDesc &desc
	) override;

	GellyOwnedInterface<IMappedBufferView> CreateMappedBufferView(
		GellyInterfaceRef<IManagedBuffer> buffer
	) override;

	GellyObserverPtr<IManagedDepthBuffer> CreateDepthBuffer(
		const DepthBufferDesc &desc
	) override;

	void DestroyTexture(const char *name) override;

	void BindMultipleTexturesAsOutput(
		GellyInterfaceVal<IManagedTexture> *textures,
		uint8_t count,
		IManagedTexture::OptionalDepthBuffer depthBuffer
	) override;

	void SetDimensions(uint16_t width, uint16_t height) override;
	void GetDimensions(uint16_t &width, uint16_t &height) override;

	void SubmitWork() override;

	void UseTextureResForNextDraw(GellyInterfaceRef<IManagedTexture> texture
	) override;

	void Draw(uint32_t vertexCount, uint32_t startVertex, bool accumulate)
		override;

	void Dispatch(
		uint32_t threadGroupCountX,
		uint32_t threadGroupCountY,
		uint32_t threadGroupCountZ
	) override;

	void ResetPipeline() override;

	void SetRasterizerFlags(RasterizerFlags flags) override;

#ifdef _DEBUG
	void PrintDebugInfo() override;
#endif
};

#endif	// GELLY_D3D11RENDERCONTEXT_H
