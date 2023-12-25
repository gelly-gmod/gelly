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

	IDirect3DDevice9Ex *device;

	float particleRadius = 5.f;
	float thresholdRatio = 10.f;

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

	FluidRenderParams renderParams = {};

	void CreateTextures();
	void LinkTextures() const;

	void UpdateRenderParams();
public:
	GellyIntegration(uint16_t width, uint16_t height, IDirect3DDevice9Ex *device);
	~GellyIntegration();

	void Render();
	void Simulate(float dt);
};

#endif //GELLY_H
