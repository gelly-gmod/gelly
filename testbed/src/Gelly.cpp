#include "Gelly.h"

#include <GellyFluidSim.h>

#include "D3D11DebugLayer.h"
#include "GellyFluidRender.h"
#include "ILogger.h"
#include "Rendering.h"
#include "Textures.h"
#include "Window.h"
#include "fluidrender/IFluidRenderer.h"
#include "fluidrender/IRenderContext.h"

using namespace testbed;

static ILogger *logger = nullptr;
static ISimContext *simContext = nullptr;
static IFluidSimulation *fluidSim = nullptr;
static IRenderContext *renderContext = nullptr;
static IFluidRenderer *fluidRenderer = nullptr;

static GellyObserverPtr<IManagedTexture> fluidAlbedoTexture;
static GellyObserverPtr<IManagedTexture> fluidDepthTexture;

constexpr int maxParticles = 1000;

void CreateGellyTextures() {
	// Basically, the process is that we use our own texture system,
	// and then we create a texture in Gelly, and then we link them by
	// using DXGI's shared surfaces. The flow is game <- Gelly. It is not
	// the reverse. Gelly will not own the textures, but it will use them.

	FeatureTextureInfo fluidAlbedoTextureInfo{};
	fluidAlbedoTextureInfo.width = WINDOW_WIDTH;
	fluidAlbedoTextureInfo.height = WINDOW_HEIGHT;
	fluidAlbedoTextureInfo.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	fluidAlbedoTextureInfo.shared = true;

	CreateFeatureTexture(GELLY_ALBEDO_TEXNAME, fluidAlbedoTextureInfo);
	fluidAlbedoTexture = renderContext->CreateSharedTexture(
		"testbed/gelly/albedo", GetTextureSharedHandle(GELLY_ALBEDO_TEXNAME)
	);

	FeatureTextureInfo fluidDepthTextureInfo{};
	fluidDepthTextureInfo.width = WINDOW_WIDTH;
	fluidDepthTextureInfo.height = WINDOW_HEIGHT;
	fluidDepthTextureInfo.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	fluidDepthTextureInfo.shared = true;

	CreateFeatureTexture(GELLY_DEPTH_TEXNAME, fluidDepthTextureInfo);
	fluidDepthTexture = renderContext->CreateSharedTexture(
		"testbed/gelly/depth", GetTextureSharedHandle(GELLY_DEPTH_TEXNAME)
	);

	// Link the textures to the fluid renderer
	fluidRenderer->GetFluidTextures()->SetFeatureTexture(
		FluidFeatureType::ALBEDO, fluidAlbedoTexture
	);

	fluidRenderer->GetFluidTextures()->SetFeatureTexture(
		FluidFeatureType::DEPTH, fluidDepthTexture
	);
}

void testbed::InitializeGelly(
	ID3D11Device *rendererDevice, ILogger *newLogger
) {
	logger = newLogger;
	try {
		logger->Info("Creating the Gelly render context...");
		renderContext =
			CreateD3D11FluidRenderContext(WINDOW_WIDTH, WINDOW_HEIGHT);

		logger->Info("Creating the Gelly fluid renderer...");
		fluidRenderer = CreateD3D11DebugFluidRenderer(renderContext);

		logger->Info("Creating the Gelly sim context...");
		simContext = CreateD3D11SimContext(
			static_cast<ID3D11Device *>(renderContext->GetRenderAPIResource(
				RenderAPIResource::D3D11Device
			)),
			static_cast<ID3D11DeviceContext *>(
				renderContext->GetRenderAPIResource(
					RenderAPIResource::D3D11DeviceContext
				)
			)
		);

		logger->Info("Creating the Gelly fluid simulation...");
		fluidSim = CreateD3D11DebugFluidSimulation(simContext);

		logger->Info("Linking the Gelly fluid simulation and renderer...");
		fluidSim->SetMaxParticles(maxParticles);
		fluidRenderer->SetSimData(fluidSim->GetSimulationData());

		fluidSim->Initialize();

		logger->Info("Creating Gelly GBuffers...");
		CreateGellyTextures();
		logger->Info("Gelly GBuffers successfully created and linked");
	} catch (const std::exception &e) {
#ifdef _DEBUG
		renderContext->PrintDebugInfo();
#endif
		logger->Error("Failed to call Gelly: %s", e.what());
		throw;
	}
	logger->Info("Gelly initialized");
}

IFluidSimulation *testbed::GetGellyFluidSim() { return fluidSim; }

IFluidRenderer *testbed::GetGellyFluidRenderer() { return fluidRenderer; }