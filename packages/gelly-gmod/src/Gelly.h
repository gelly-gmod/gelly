#ifndef GELLY_H
#define GELLY_H

#include <GellyFluidRender.h>
#include <GellyFluidSim.h>

#include <memory>
#include <thread>

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

	float particleRadius = 1.75f;
	float thresholdRatio = 10.f;
	bool isSimulationInteractive = false;

	struct {
		IDirect3DTexture9* depthTexture;
		IDirect3DTexture9* albedoTexture;
		IDirect3DTexture9* normalTexture;
		IDirect3DTexture9* positionTexture;
		IDirect3DTexture9* thicknessTexture;
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

	IDirect3DStateBlock9* stateBlock = nullptr;
	FluidRenderParams renderParams = {};

	void CreateShaders();
	void CreateBuffers();
	void CreateTextures();
	void LinkTextures() const;

	void UpdateRenderParams();
public:
	GellyIntegration(uint16_t width, uint16_t height, IDirect3DDevice9Ex *device);
	~GellyIntegration();

	void Render();
	void Simulate(float dt);
	void LoadMap(const char *mapName);

	[[nodiscard]] IFluidSimulation *GetSimulation() const;
};

#endif //GELLY_H
