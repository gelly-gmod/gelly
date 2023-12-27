#ifndef GELLY_H
#define GELLY_H

#include <GellyFluidRender.h>
#include <GellyFluidSim.h>

#include <memory>
#include <thread>

struct FluidVisualParams {
	// 4th member is shininess
	float absorption[4] = { 0.3f, 0.3f, 0.f, 1024.f };
	float refractionStrength = 0.03f;
	float pad0[3];
};


struct CompositeConstants {
	float eyePos[3];
	float pad0;
	FluidVisualParams fluidParams;
};

static_assert(sizeof(CompositeConstants) % 16 == 0);

/**
 * \brief Holds together the fluid simulation and rendering.
 */
class GellyIntegration {
private:
	IFluidRenderer *renderer;
	IFluidSimulation *simulation;
	IRenderContext *renderContext;
	ISimContext *simContext;
	ObjectHandle mapHandle = INVALID_OBJECT_HANDLE;

	IDirect3DDevice9Ex *device;

	float particleRadius = 0.3f;
	float thresholdRatio = 3.f;
	bool isSimulationInteractive = false;

	struct {
		IDirect3DTexture9* depthTexture;
		IDirect3DTexture9* albedoTexture;
		IDirect3DTexture9* normalTexture;
		IDirect3DTexture9* positionTexture;
		IDirect3DTexture9* thicknessTexture;

		IDirect3DTexture9* backbufferTexture;
	} textures;

	struct {
		GellyObserverPtr<IManagedTexture> depthTexture;
		GellyObserverPtr<IManagedTexture> albedoTexture;
		GellyObserverPtr<IManagedTexture> normalTexture;
		GellyObserverPtr<IManagedTexture> positionTexture;
		GellyObserverPtr<IManagedTexture> thicknessTexture;
	} gellyTextures;

	struct {
		// All of these are requried to be NULL.
		HANDLE depthTexture = nullptr;
		HANDLE albedoTexture = nullptr;
		HANDLE normalTexture = nullptr;
		HANDLE positionTexture = nullptr;
		HANDLE thicknessTexture = nullptr;
	} sharedHandles;

	struct {
		IDirect3DPixelShader9* compositePS;
		IDirect3DVertexShader9* ndcQuadVS;
	} shaders;

	struct NDCVertex {
		float x, y, z, w;
		float texX, texY;
	};

	struct {
		IDirect3DVertexBuffer9* ndcQuadVB;
	} buffers;

	CompositeConstants compositeConstants = {};
	IDirect3DStateBlock9* stateBlock = nullptr;
	FluidRenderParams renderParams = {};

	void CreateShaders();
	void CreateBuffers();
	void CreateTextures();
	void LinkTextures() const;

	void UpdateRenderParams();
	void SetCompositeConstants();
public:
	GellyIntegration(uint16_t width, uint16_t height, IDirect3DDevice9Ex *device);
	~GellyIntegration();

	void Render();
	void Simulate(float dt);
	void LoadMap(const char *mapName);
	void SetFluidParams(const FluidVisualParams &params);

	[[nodiscard]] const char* GetComputeDeviceName() const;
	[[nodiscard]] IFluidSimulation *GetSimulation() const;
};

#endif //GELLY_H
