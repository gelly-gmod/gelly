#ifndef GELLY_GELLYRENDERER_H
#define GELLY_GELLYRENDERER_H

#include <GellyD3D.h>
#include <d3d11.h>
#include <directxmath.h>
#include <windows.h>
#include <wrl.h>

#include "detail/Camera.h"
#include "detail/ConstantBuffer.h"
#include "detail/DataTypes.h"
#include "detail/GBuffer.h"
#include "rendering/Technique.h"
#include "rendering/techniques/NormalSmoothing.h"
#include "rendering/techniques/ParticleRendering.h"

using namespace Microsoft::WRL;
using namespace DirectX;

struct SharedTextures {
	d3d9::Texture *depth;
	d3d9::Texture *normal;
};

struct RendererInitParams {
	int maxParticles;
	int width;
	int height;
	SharedTextures sharedTextures;
};

class RendererResources {
public:
	GBuffer gbuffer;

	struct {
		ComPtr<ID3D11DepthStencilView> view;
		ComPtr<ID3D11Texture2D> buffer;
		ComPtr<ID3D11DepthStencilState> state;
	} depthStencil;

	RendererResources(ID3D11Device *device, const RendererInitParams &params);
	~RendererResources() = default;
};

class GellyRenderer {
private:
#ifdef _DEBUG
	ComPtr<ID3D11InfoQueue> debugMsgQueue;
#endif
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> deviceContext;
	ComPtr<ID3D11RasterizerState> rasterizerState;
	ComPtr<ID3D11Buffer> particles;

	RendererResources *resources;
	RendererInitParams params;

	struct {
		ParticleRendering *particleRendering;
		NormalSmoothing *normalEstimation;
	} pipeline;

	int activeParticles{};

	/**
	 * Initializes the techniques and their resources.
	 */
	void InitializePipeline();

public:
	Camera camera;

	/**
	 * This retrieves the D3D11 buffer which contains the particle data.
	 * @note This is meant for doing GPU-GPU copying with FleX.
	 * @return
	 */
	[[nodiscard]] ID3D11Buffer *GetD3DParticleBuffer() const;
	[[nodiscard]] ID3D11Device *GetD3DDevice() const;

	void Render();

#ifdef _DEBUG
	void PrintDebugMessages();
#endif

	void SetActiveParticles(int newActiveParticles);

	explicit GellyRenderer(const RendererInitParams &params);
	~GellyRenderer();
};

GellyRenderer *GellyRenderer_Create(const RendererInitParams &params);
void GellyRenderer_Destroy(GellyRenderer *renderer);

#endif	// GELLY_GELLYRENDERER_H
