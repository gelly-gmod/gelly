#include "Gelly.h"

#include <GellyFluidSim.h>

#include "D3D11DebugLayer.h"
#include "GellyFluidRender.h"
#include "ILogger.h"
#include "Rendering.h"
#include "Scene.h"
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
static FluidRenderSettings fluidRenderSettings{};
static GellySimMode currentSimMode = GellySimMode::DEBUG;

static GellyObserverPtr<IManagedTexture> fluidAlbedoTexture;
static GellyObserverPtr<IManagedTexture> fluidDepthTexture;

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
		"testbed/gelly/albedo",
		GetTextureSharedHandle(GELLY_ALBEDO_TEXNAME),
		ContextRenderAPI::D3D11
	);

	FeatureTextureInfo fluidDepthTextureInfo{};
	fluidDepthTextureInfo.width = WINDOW_WIDTH;
	fluidDepthTextureInfo.height = WINDOW_HEIGHT;
	fluidDepthTextureInfo.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	fluidDepthTextureInfo.shared = true;

	CreateFeatureTexture(GELLY_DEPTH_TEXNAME, fluidDepthTextureInfo);
	fluidDepthTexture = renderContext->CreateSharedTexture(
		"testbed/gelly/depth",
		GetTextureSharedHandle(GELLY_DEPTH_TEXNAME),
		ContextRenderAPI::D3D11
	);

	FeatureTextureInfo fluidNormalsTextureInfo{};
	fluidNormalsTextureInfo.width = WINDOW_WIDTH;
	fluidNormalsTextureInfo.height = WINDOW_HEIGHT;
	fluidNormalsTextureInfo.format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	fluidNormalsTextureInfo.shared = true;

	CreateFeatureTexture(GELLY_NORMAL_TEXNAME, fluidNormalsTextureInfo);
	auto fluidNormalsTexture = renderContext->CreateSharedTexture(
		"testbed/gelly/normals",
		GetTextureSharedHandle(GELLY_NORMAL_TEXNAME),
		ContextRenderAPI::D3D11
	);

	FeatureTextureInfo fluidPositionsTextureInfo{};
	fluidPositionsTextureInfo.width = WINDOW_WIDTH;
	fluidPositionsTextureInfo.height = WINDOW_HEIGHT;
	fluidPositionsTextureInfo.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	fluidPositionsTextureInfo.shared = true;

	CreateFeatureTexture(GELLY_POSITIONS_TEXNAME, fluidPositionsTextureInfo);
	auto fluidPositionsTexture = renderContext->CreateSharedTexture(
		"testbed/gelly/positions",
		GetTextureSharedHandle(GELLY_POSITIONS_TEXNAME),
		ContextRenderAPI::D3D11
	);

	FeatureTextureInfo fluidThicknessTextureInfo{};
	fluidThicknessTextureInfo.width = LOWRES_WINDOW_WIDTH;
	fluidThicknessTextureInfo.height = LOWRES_WINDOW_HEIGHT;
	fluidThicknessTextureInfo.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	fluidThicknessTextureInfo.shared = true;

	CreateFeatureTexture(GELLY_THICKNESS_TEXNAME, fluidThicknessTextureInfo);
	auto fluidThicknessTexture = renderContext->CreateSharedTexture(
		"testbed/gelly/thickness",
		GetTextureSharedHandle(GELLY_THICKNESS_TEXNAME),
		ContextRenderAPI::D3D11
	);

	// Link the textures to the fluid renderer
	fluidRenderer->GetFluidTextures()->SetFeatureTexture(
		FluidFeatureType::ALBEDO, fluidAlbedoTexture
	);

	fluidRenderer->GetFluidTextures()->SetFeatureTexture(
		FluidFeatureType::DEPTH, fluidDepthTexture
	);

	fluidRenderer->GetFluidTextures()->SetFeatureTexture(
		FluidFeatureType::NORMALS, fluidNormalsTexture
	);

	fluidRenderer->GetFluidTextures()->SetFeatureTexture(
		FluidFeatureType::POSITIONS, fluidPositionsTexture
	);

	fluidRenderer->GetFluidTextures()->SetFeatureTexture(
		FluidFeatureType::THICKNESS, fluidThicknessTexture
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

		renderContext->SetDimensions(WINDOW_WIDTH, WINDOW_HEIGHT);
		logger->Info("Creating the Gelly fluid renderer...");
		fluidRenderer = CreateD3D11SplattingFluidRenderer(renderContext);
		// fluidRenderer = CreateD3D11IsosurfaceFluidRenderer(renderContext);

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

		logger->Info("Creating Gelly GBuffers...");
		CreateGellyTextures();
		logger->Info("Gelly GBuffers successfully created and linked");

		InitializeNewGellySim(
			GellySimInit{GellySimMode::DEBUG, GellySimInit::DebugInfo{10000}}
		);

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
		logger->Info("Enabling Gelly RenderDoc captures...");
		if (!fluidRenderer->EnableRenderDocCaptures()) {
			logger->Warning(
				"Failed to enable Gelly RenderDoc captures. This is normal "
				"if RenderDoc is not running or if the API has changed."
			);
#endif
		}
		catch (const std::exception &e) {
#ifdef _DEBUG
			renderContext->PrintDebugInfo();
#endif
			logger->Error("Failed to call Gelly: %s", e.what());
			throw;
		}
		logger->Info("Gelly initialized");
	}

	void testbed::InitializeNewGellySim(const GellySimInit &init) {
		try {
			if (fluidSim) {
				DestroyGellyFluidSim(fluidSim);
				logger->Info("Destroyed old Gelly simulation");
				fluidSim = nullptr;
			}

			logger->Info("Initializing new Gelly simulation...");
			switch (init.mode) {
				case GellySimMode::DEBUG: {
					const auto &debugInfo =
						std::get<GellySimInit::DebugInfo>(init.modeInfo);

					fluidSim = CreateD3D11DebugFluidSimulation(simContext);
					fluidSim->SetMaxParticles(debugInfo.maxParticles);
					break;
				}

				case GellySimMode::RTFR: {
					const auto &rtfrInfo =
						std::get<GellySimInit::RTFRInfo>(init.modeInfo);

					fluidSim = CreateD3D11RTFRFluidSimulation(
						simContext, rtfrInfo.folderPath
					);
					break;
				}

				case GellySimMode::FLEX: {
					const auto &flexInfo =
						std::get<GellySimInit::FlexInfo>(init.modeInfo);

					fluidSim = CreateD3D11FlexFluidSimulation(simContext);
					fluidSim->SetMaxParticles(flexInfo.maxParticles);
					break;
				}
			}

			currentSimMode = init.mode;

			logger->Info("Linking the Gelly fluid simulation and renderer...");
			fluidRenderer->SetSimData(fluidSim->GetSimulationData());
			fluidSim->Initialize();
			logger->Info("Gelly simulation initialized");

			if (!fluidSim->GetScene()) {
				logger->Warning("Simulation has no interactive scene.");
			} else {
				// tell scene system to register
				RegisterSceneToGellySim(fluidSim);
			}
		} catch (const std::exception &e) {
			logger->Error(
				"Failed to initialize Gelly simulation: %s", e.what()
			);
			throw;
		}
	}

	IFluidSimulation *testbed::GetGellyFluidSim() { return fluidSim; }

	GellySimMode testbed::GetCurrentGellySimMode() { return currentSimMode; }

	IFluidRenderer *testbed::GetGellyFluidRenderer() { return fluidRenderer; }

	IRenderContext *testbed::GetGellyRenderContext() { return renderContext; }

	Gelly::FluidRenderSettings testbed::GetGellyFluidRenderSettings() {
		return fluidRenderSettings;
	}

	void testbed::UpdateGellyFluidRenderSettings(
		const Gelly::FluidRenderSettings &settings
	) {
		fluidRenderSettings = settings;
		fluidRenderer->SetSettings(settings);
	}