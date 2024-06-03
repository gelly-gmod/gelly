#ifndef GELLY_CD3D11DEBUGFLUIDRENDERER_H
#define GELLY_CD3D11DEBUGFLUIDRENDERER_H

#include <GellyFluidSim.h>
#include <GellyInterfaceRef.h>

#include <array>

#include "CD3D11SplattingFluidTextures.h"
#include "fluidrender/CD3D11ManagedBuffer.h"
#include "fluidrender/IFluidRenderer.h"
#include "fluidrender/IManagedBufferLayout.h"
#include "renderdoc_app.h"

class CD3D11SplattingFluidRenderer : public IFluidRenderer {
private:
	GellyInterfaceVal<IRenderContext> context;
	/**
	 * The particle data comes from here, but the renderer does not own it.
	 */
	GellyInterfaceVal<ISimData> simData;
	GellyOwnedInterface<IPerfMarker> perfMarker;
	CD3D11SplattingFluidTextures outputTextures;

	Gelly::FluidRenderSettings settings = {};
	int maxParticles{};

	FluidRenderParams cbufferData{};

	bool lowBitMode = false;

	struct {
		GellyInterfaceVal<IManagedBuffer> positions;
		GellyInterfaceVal<IManagedBuffer> foamPositions;
		GellyInterfaceVal<IManagedBuffer> foamVelocities;

		GellyInterfaceVal<IManagedBuffer> anisotropyQ1;
		GellyInterfaceVal<IManagedBuffer> anisotropyQ2;
		GellyInterfaceVal<IManagedBuffer> anisotropyQ3;

		GellyInterfaceVal<IManagedBufferLayout> splattingLayout;
		GellyInterfaceVal<IManagedBufferLayout> foamLayout;

		GellyInterfaceVal<IManagedBuffer> fluidRenderCBuffer;
		GellyInterfaceVal<IManagedDepthBuffer> depthBuffer;

		GellyInterfaceVal<IManagedBuffer> screenQuad;
		GellyInterfaceVal<IManagedBufferLayout> screenQuadLayout;

		GellyInterfaceVal<IManagedBuffer> particleAbsorption;
	} buffers;

	struct {
		GellyOwnedInterface<IMappedBufferView> absorptionView;
	} views;

	struct {
		GellyInterfaceVal<IManagedTexture> unfilteredDepth;
		GellyInterfaceVal<IManagedTexture> unfilteredThickness;
		GellyInterfaceVal<IManagedTexture> unfilteredAlbedo;
		/**
		 * \brief When in low-bit mode, this texture is the one in the filter
		 * flipping instead of the output texture. This is so that all of our
		 * internal processes can continue to enjoy high precision, but we can
		 * output an encoded depth at the final stage.
		 */
		GellyInterfaceVal<IManagedTexture> untransformedDepth;
	} internalTextures{};

#ifndef PRODUCTION_BUILD
	/*
	 * \brief Random access AOVs for debugging GPU algorithms, each are random
	 * access four-channel textures that are 32-bit floats.
	 */
	std::array<GellyInterfaceVal<IManagedTexture>, 4> randomAccessAOVs;
#endif

	struct {
		GellyInterfaceVal<IManagedShader> splattingPS;
		GellyInterfaceVal<IManagedShader> splattingVS;
		GellyInterfaceVal<IManagedShader> splattingGS;

		GellyInterfaceVal<IManagedShader> thicknessPS;
		GellyInterfaceVal<IManagedShader> thicknessVS;
		GellyInterfaceVal<IManagedShader> thicknessGS;

		GellyInterfaceVal<IManagedShader> foamPS;
		GellyInterfaceVal<IManagedShader> foamVS;
		GellyInterfaceVal<IManagedShader> foamGS;

		GellyInterfaceVal<IManagedShader> screenQuadVS;

		GellyInterfaceVal<IManagedShader> filterDepthPS;
		GellyInterfaceVal<IManagedShader> estimateNormalPS;
		GellyInterfaceVal<IManagedShader> filterThicknessPS;
		GellyInterfaceVal<IManagedShader> encodeDepthPS;
	} shaders{};
#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	RENDERDOC_API_1_1_2 *renderDocApi = nullptr;
#endif

	void CreateBuffers();
	void CreateTextures();
	void CreateShaders();

	void RenderUnfilteredDepth();
	void RenderFilteredDepth();
	void RenderNormals();
	void RenderThickness();
	void RenderFoam(bool depthOnly);
	void RenderGenericBlur(
		GellyInterfaceVal<IManagedTexture> texA,
		GellyInterfaceVal<IManagedTexture> texB
	);
	void EncodeDepth();

public:
	CD3D11SplattingFluidRenderer();
	~CD3D11SplattingFluidRenderer() override = default;

	void SetSimData(GellyObserverPtr<ISimData> simData) override;
	void AttachToContext(GellyObserverPtr<IRenderContext> context) override;
	GellyObserverPtr<IFluidTextures> GetFluidTextures() override;

	void Render() override;
	void EnableLowBitMode() override;

	void SetSettings(const Gelly::FluidRenderSettings &settings) override;
	void SetPerFrameParams(const Gelly::FluidRenderParams &params) override;

	void PullPerParticleData() override;
	void SetPerParticleAbsorption(uint particleIndex, const float absorption[3])
		override;
	void PushPerParticleData() override;

	bool CheckFeatureSupport(GELLY_FEATURE feature) override;

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	bool EnableRenderDocCaptures() override;
#endif
};

#endif	// GELLY_CD3D11DEBUGFLUIDRENDERER_H
