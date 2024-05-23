#ifndef STANDARDPIPELINE_H
#define STANDARDPIPELINE_H

#include <DirectXMath.h>

#include "../Pipeline.h"
#include "StandardTextures.h"
#include "fluidrender/IRenderContext.h"
#include "source/GetCubemap.h"
#include "source/MathTypes.h"

using namespace DirectX;

struct CompositeConstants {
	float eyePos[3];
	float pad0;
	float refractionStrength = 0.03f;
	float cubemapStrength = 1.f;
	float pad1[2];

	struct CompositeLight {
		/**
		 * XYZ - Color,
		 * W - Range
		 */
		float lightInfo[4];
		float position[3];
		float pad0;
		/**
		 * 0.0 == disabled, 1.0 == enabled
		 * (a boolean always misaligns the struct)
		 */
		float enabled = 0.f;
		float pad[3];
	};

	CompositeLight lights[2];
	float aspectRatio;
	float pad2[3];

	AmbientLightCube ambientLightCube;
	PipelineFluidMaterial material;
};

static_assert(sizeof(CompositeConstants) % 16 == 0);
class StandardPipeline : public Pipeline {
private:
	struct NDCVertex {
		float x, y, z, w;
		float texX, texY;
	};

	GellyResources gellyResources;
	UnownedResources gmodResources;

	CompositeConstants compositeConstants;

	std::optional<StandardTextures> textures;
	ComPtr<IDirect3DTexture9> backBuffer;
	ComPtr<IDirect3DVertexBuffer9> ndcQuad;
	ComPtr<IDirect3DPixelShader9> compositeShader;
	ComPtr<IDirect3DPixelShader9> compositeFoamShader;
	ComPtr<IDirect3DVertexShader9> quadVertexShader;
	ComPtr<IDirect3DStateBlock9> stateBlock;

	PipelineConfig config;
	PipelineFluidMaterial fluidMaterial;

	void CreateCompositeShader();
	void CreateQuadVertexShader();
	void CreateNDCQuad();
	void CreateStateBlock();
	void CreateBackBuffer();

	void UpdateBackBuffer() const;
	void SetCompositeShaderConstants() const;

	void UpdateGellyRenderParams();
	void RenderGellyFrame();

	void SetCompositeSamplerState(int index, D3DTEXTUREFILTERTYPE filter) const;
	void CompositeFoam(bool withGellyRendered) const;

public:
	StandardPipeline();
	~StandardPipeline() override;

	void CreatePipelineLocalResources(
		const GellyResources &gelly, const UnownedResources &gmod
	) override;

	void SetConfig(const PipelineConfig &config) override;
	[[nodiscard]] PipelineConfig GetConfig() const override;

	void SetFluidMaterial(const PipelineFluidMaterial &material) override;

	void Composite() override;

	void Render() override;
};

#endif	// STANDARDPIPELINE_H
