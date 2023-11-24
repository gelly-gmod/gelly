#ifndef GELLY_CD3D11DEBUGFLUIDRENDERER_H
#define GELLY_CD3D11DEBUGFLUIDRENDERER_H

#include <GellyFluidSim.h>
#include <GellyInterfaceRef.h>

#include "CD3D11DebugFluidTextures.h"
#include "CD3D11ManagedBuffer.h"
#include "IFluidRenderer.h"
#include "IManagedBufferLayout.h"

class CD3D11DebugFluidRenderer : public IFluidRenderer {
private:
	struct FluidRenderCBuffer {
		XMFLOAT4X4 view;
		XMFLOAT4X4 proj;
		XMFLOAT4X4 invView;
		XMFLOAT4X4 invProj;
	};

	GellyInterfaceVal<IRenderContext> context;
	/**
	 * The particle data comes from here, but the renderer does not own it.
	 */
	GellyInterfaceVal<ISimData> simData;
	CD3D11DebugFluidTextures outputTextures;

	Gelly::FluidRenderSettings settings;
	int maxParticles{};

	FluidRenderCBuffer cbufferData{};

	struct {
		GellyInterfaceVal<IManagedBuffer> positions;
		GellyInterfaceVal<IManagedBufferLayout> positionsLayout;
		GellyInterfaceVal<IManagedBuffer> fluidRenderCBuffer;
	} buffers;

	struct {
		GellyInterfaceVal<IManagedTexture> unfilteredDepth;
	} internalTextures{};

	struct {
		GellyInterfaceVal<IManagedShader> splattingPS;
		GellyInterfaceVal<IManagedShader> splattingVS;
		GellyInterfaceVal<IManagedShader> splattingGS;
	} shaders{};

	void CreateBuffers();
	void CreateTextures();
	void CreateShaders();

public:
	CD3D11DebugFluidRenderer();
	~CD3D11DebugFluidRenderer() override = default;

	void SetSimData(GellyObserverPtr<ISimData> simData) override;
	void AttachToContext(GellyObserverPtr<IRenderContext> context) override;
	GellyObserverPtr<IFluidTextures> GetFluidTextures() override;
	void Render() override;

	void SetSettings(const Gelly::FluidRenderSettings &settings) override;
	void SetPerFrameParams(const Gelly::FluidRenderParams &params) override;
};

#endif	// GELLY_CD3D11DEBUGFLUIDRENDERER_H
