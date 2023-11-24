#ifndef GELLY_CD3D11DEBUGFLUIDRENDERER_H
#define GELLY_CD3D11DEBUGFLUIDRENDERER_H

#include <GellyFluidSim.h>

#include "CD3D11DebugFluidTextures.h"
#include "CD3D11ManagedBuffer.h"
#include "IFluidRenderer.h"
#include "IManagedBufferLayout.h"

class CD3D11DebugFluidRenderer : public IFluidRenderer {
private:
	GellyObserverPtr<IRenderContext> context;
	/**
	 * The particle data comes from here, but the renderer does not own it.
	 */
	GellyObserverPtr<ISimData> simData;
	CD3D11DebugFluidTextures outputTextures;

	Gelly::FluidRenderSettings settings;
	int maxParticles{};

	struct {
		GellyObserverPtr<IManagedBuffer> positions;
		GellyObserverPtr<IManagedBufferLayout> positionsLayout;
	} buffers;

	struct {
		GellyObserverPtr<IManagedTexture> unfilteredDepth;
	} internalTextures{};

	void CreateBuffers();
	void CreateTextures();

public:
	CD3D11DebugFluidRenderer();
	~CD3D11DebugFluidRenderer() override = default;

	void SetSimData(GellyObserverPtr<ISimData> simData) override;
	void AttachToContext(GellyObserverPtr<IRenderContext> context) override;
	GellyObserverPtr<IFluidTextures> GetFluidTextures() override;
	void Render() override;

	void SetSettings(const Gelly::FluidRenderSettings &settings) override;
};

#endif	// GELLY_CD3D11DEBUGFLUIDRENDERER_H
