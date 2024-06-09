#include "StandardPipeline.h"

#include <d3d9.h>

#include "../../logging/global-macros.h"
#include "fluidrender/CD3D11RenderContext.h"
#include "shaders/out/CompositeFoamPS.h"
#include "shaders/out/CompositePS.h"
#include "shaders/out/NDCQuadVS.h"
#include "source/CViewSetup.h"
#include "source/GetCubemap.h"
#include "source/IBaseClientDLL.h"
#include "source/IVRenderView.h"

using namespace gsc;

void StandardPipeline::CreateCompositeShader() {
	auto &device = gmodResources.device;

	if (const auto hr = device->CreatePixelShader(
			reinterpret_cast<const DWORD *>(CompositePS::GetBytecode()),
			compositeShader.GetAddressOf()
		);
		FAILED(hr)) {
		throw std::runtime_error("Failed to create composite shader");
	}

	if (const auto hr = device->CreatePixelShader(
			reinterpret_cast<const DWORD *>(CompositeFoamPS::GetBytecode()),
			compositeFoamShader.GetAddressOf()
		);
		FAILED(hr)) {
		throw std::runtime_error("Failed to create composite foam shader");
	}
}

void StandardPipeline::CreateQuadVertexShader() {
	auto &device = gmodResources.device;

	if (const auto hr = device->CreateVertexShader(
			reinterpret_cast<const DWORD *>(NDCQuadVS::GetBytecode()),
			quadVertexShader.GetAddressOf()
		);
		FAILED(hr)) {
		throw std::runtime_error("Failed to create quad vertex shader");
	}
}

void StandardPipeline::CreateNDCQuad() {
	auto &device = gmodResources.device;
	NDCVertex vertices[] = {
		{-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
		{-1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f}
	};

	if (const auto hr = device->CreateVertexBuffer(
			sizeof(vertices),
			D3DUSAGE_WRITEONLY,
			0,
			D3DPOOL_DEFAULT,
			ndcQuad.GetAddressOf(),
			nullptr
		);
		FAILED(hr)) {
		throw std::runtime_error("Failed to create NDC quad");
	}

	void *vertexBufferPtr = nullptr;
	if (const auto hr = ndcQuad->Lock(0, sizeof(vertices), &vertexBufferPtr, 0);
		FAILED(hr)) {
		throw std::runtime_error("Failed to lock NDC quad");
	}

	std::memcpy(vertexBufferPtr, vertices, sizeof(vertices));

	if (const auto hr = ndcQuad->Unlock(); FAILED(hr)) {
		throw std::runtime_error("Failed to unlock NDC quad");
	}
}

void StandardPipeline::CreateStateBlock() {
	auto &device = gmodResources.device;

	if (const auto hr =
			device->CreateStateBlock(D3DSBT_ALL, stateBlock.GetAddressOf());
		FAILED(hr)) {
		throw std::runtime_error("Failed to create state block");
	}
}

void StandardPipeline::CreateBackBuffer() {
	auto &device = gmodResources.device;

	uint16_t width, height;
	gellyResources.context->GetDimensions(width, height);

	if (const auto hr = device->CreateTexture(
			width,
			height,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			backBuffer.GetAddressOf(),
			nullptr
		);
		FAILED(hr)) {
		throw std::runtime_error("Failed to create back buffer");
	}
}

void StandardPipeline::UpdateBackBuffer() const {
	auto &device = gmodResources.device;

	IDirect3DSurface9 *backBufferSurface;
	IDirect3DSurface9 *backBufferTextureSurface;

	if (const auto hr = device->GetRenderTarget(0, &backBufferSurface);
		FAILED(hr)) {
		throw std::runtime_error("Failed to get back buffer surface");
	}

	if (const auto hr =
			backBuffer->GetSurfaceLevel(0, &backBufferTextureSurface);
		FAILED(hr)) {
		throw std::runtime_error("Failed to get back buffer texture surface");
	}

	if (const auto hr = device->StretchRect(
			backBufferSurface,
			nullptr,
			backBufferTextureSurface,
			nullptr,
			D3DTEXF_NONE
		);
		FAILED(hr)) {
		throw std::runtime_error("Failed to stretch rect");
	}
}

void StandardPipeline::SetCompositeShaderConstants() const {
	auto &device = gmodResources.device;

	if (const auto hr = device->SetPixelShaderConstantF(
			0,
			reinterpret_cast<const float *>(&compositeConstants),
			sizeof(compositeConstants) / sizeof(float) / 4
		);
		FAILED(hr)) {
		throw std::runtime_error("Failed to set composite shader constants");
	}
}

void StandardPipeline::UpdateGellyRenderParams() {
	auto &renderer = gellyResources.renderer;

	CViewSetup viewSetup;
	GetClientViewSetup(viewSetup);

	XMFLOAT4X4 viewMatrix = {};
	XMFLOAT4X4 projectionMatrix = {};
	XMFLOAT4X4 _unused = {};
	XMFLOAT4X4 _unused1 = {};

	GetMatricesFromView(
		viewSetup,
		reinterpret_cast<VMatrix *>(&viewMatrix),
		reinterpret_cast<VMatrix *>(&projectionMatrix),
		reinterpret_cast<VMatrix *>(&_unused),
		reinterpret_cast<VMatrix *>(&_unused1)
	);

	XMFLOAT4X4 inverseViewMatrix = {};
	XMStoreFloat4x4(
		&inverseViewMatrix,
		XMMatrixInverse(nullptr, XMLoadFloat4x4(&viewMatrix))
	);

	XMFLOAT4X4 inverseProjectionMatrix = {};
	XMStoreFloat4x4(
		&inverseProjectionMatrix,
		XMMatrixInverse(nullptr, XMLoadFloat4x4(&projectionMatrix))
	);

	// Transpose all matrices
	XMStoreFloat4x4(
		&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix))
	);

	XMStoreFloat4x4(
		&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix))
	);

	XMStoreFloat4x4(
		&inverseViewMatrix,
		XMMatrixTranspose(XMLoadFloat4x4(&inverseViewMatrix))
	);

	XMStoreFloat4x4(
		&inverseProjectionMatrix,
		XMMatrixTranspose(XMLoadFloat4x4(&inverseProjectionMatrix))
	);

	FluidRenderParams renderParams = {};
	renderParams.view = viewMatrix;
	renderParams.proj = projectionMatrix;
	renderParams.invView = inverseViewMatrix;
	renderParams.invProj = inverseProjectionMatrix;

	renderParams.particleRadius = config.particleRadius;
	renderParams.thresholdRatio = config.thresholdRatio;
	renderParams.diffuseScale = config.diffuseScale;
	renderParams.diffuseMotionBlur = config.diffuseMotionBlur;

	renderParams.width = static_cast<float>(viewSetup.width);
	renderParams.height = static_cast<float>(viewSetup.height);
	renderParams.farPlane = viewSetup.zFar;
	renderParams.nearPlane = viewSetup.zNear;

	renderParams.cameraPos.x = viewSetup.origin.x;
	renderParams.cameraPos.y = viewSetup.origin.y;
	renderParams.cameraPos.z = viewSetup.origin.z;

	compositeConstants.eyePos[0] = viewSetup.origin.x;
	compositeConstants.eyePos[1] = viewSetup.origin.y;
	compositeConstants.eyePos[2] = viewSetup.origin.z;

	compositeConstants.cubemapStrength = config.cubemapStrength;
	compositeConstants.refractionStrength = config.refractionStrength;

	for (int index = 1; index < 3; index++) {
		auto light = GetLightDesc(index);

		// Create a local index from 1-2 to 0-1 (lights 1-2 are the only ones
		// that affect specular lighting in GMod) source: empirical testing

		int lightIndex = index - 1;
		if (!light || light->m_Type != MATERIAL_LIGHT_POINT) {
			compositeConstants.lights[lightIndex].enabled = 0.f;
			continue;
		}

		compositeConstants.lights[lightIndex].enabled = 1.f;
		compositeConstants.lights[lightIndex].position[0] = light->m_Position.x;
		compositeConstants.lights[lightIndex].position[1] = light->m_Position.y;
		compositeConstants.lights[lightIndex].position[2] = light->m_Position.z;
		compositeConstants.lights[lightIndex].lightInfo[0] = light->m_Color.x;
		compositeConstants.lights[lightIndex].lightInfo[1] = light->m_Color.y;
		compositeConstants.lights[lightIndex].lightInfo[2] = light->m_Color.z;
		compositeConstants.lights[lightIndex].lightInfo[3] = light->m_Range;
	}

	compositeConstants.aspectRatio =
		renderParams.width / renderParams.height;  // viewport aspect ratio

	renderer->SetPerFrameParams(renderParams);

	std::memcpy(
		&compositeConstants.ambientLightCube,
		GetAmbientLightCube(),
		sizeof(AmbientLightCube)
	);

	compositeConstants.material = fluidMaterial;
}

void StandardPipeline::RenderGellyFrame() {
	auto &renderer = gellyResources.renderer;

	FluidRenderSettings settings = {};
	settings.filterIterations = config.filterIterations;
	settings.thicknessFilterIterations = config.thicknessIterations;

	renderer->SetSettings(settings);
	renderer->Render();

#ifdef GELLY_USE_DEBUG_LAYER
	auto *d3d11Context =
		reinterpret_cast<CD3D11RenderContext *>(gellyResources.context.get());
	d3d11Context->PrintDebugInfo();
#endif
}

void StandardPipeline::SetCompositeSamplerState(
	int index, D3DTEXTUREFILTERTYPE filter, bool srgb = false
) const {
	auto &device = gmodResources.device;
	device->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	device->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	device->SetSamplerState(index, D3DSAMP_MINFILTER, filter);
	device->SetSamplerState(index, D3DSAMP_MAGFILTER, filter);
	device->SetSamplerState(index, D3DSAMP_MIPFILTER, filter);

	// we also need to make sure to convert sRGB to linear
	device->SetSamplerState(index, D3DSAMP_SRGBTEXTURE, srgb);
}

StandardPipeline::StandardPipeline()
	: Pipeline(),
	  gellyResources(),
	  gmodResources(),
	  textures(),
	  backBuffer(),
	  ndcQuad(),
	  compositeShader(),
	  quadVertexShader() {}

StandardPipeline::~StandardPipeline() { RemoveAmbientLightCubeHooks(); }

void StandardPipeline::CreatePipelineLocalResources(
	const GellyResources &gelly, const UnownedResources &gmod
) {
	gellyResources = gelly;
	gmodResources = gmod;

	textures.emplace(gellyResources, gmodResources);
	CreateCompositeShader();
	CreateQuadVertexShader();
	CreateNDCQuad();
	CreateStateBlock();
	CreateBackBuffer();
}

void StandardPipeline::SetConfig(const PipelineConfig &config) {
	this->config = config;
}

PipelineConfig StandardPipeline::GetConfig() const { return config; }

void StandardPipeline::SetFluidMaterial(const PipelineFluidMaterial &material) {
	fluidMaterial = material;
}

void StandardPipeline::CompositeFoam(bool withGellyRendered) const {
	auto &device = gmodResources.device;

	stateBlock->Capture();

	SetCompositeShaderConstants();
	device->SetVertexShader(quadVertexShader.Get());
	device->SetPixelShader(compositeFoamShader.Get());

	SetCompositeSamplerState(0, D3DTEXF_POINT);

	device->SetTexture(0, textures->gmodTextures.foam.Get());
	device->SetStreamSource(0, ndcQuad.Get(), 0, sizeof(NDCVertex));
	device->SetFVF(D3DFVF_XYZW | D3DFVF_TEX1);

	device->SetRenderState(D3DRS_ZENABLE, TRUE);
	device->SetRenderState(
		D3DRS_ZWRITEENABLE, withGellyRendered ? TRUE : FALSE
	);

	// We do actually want to use an alpha blend here
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	device->SetRenderState(D3DRS_SRGBWRITEENABLE, TRUE);
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	stateBlock->Apply();
}

void StandardPipeline::Composite() {
	auto &device = gmodResources.device;

	CompositeFoam(false);  // so that it can be seen in water
	UpdateBackBuffer();

	stateBlock->Capture();

	SetCompositeShaderConstants();
	device->SetVertexShader(quadVertexShader.Get());
	device->SetPixelShader(compositeShader.Get());

	SetCompositeSamplerState(0, D3DTEXF_POINT);
	SetCompositeSamplerState(1, D3DTEXF_POINT);
	SetCompositeSamplerState(2, D3DTEXF_POINT);
	SetCompositeSamplerState(3, D3DTEXF_POINT, true);
	SetCompositeSamplerState(4, D3DTEXF_LINEAR);
	SetCompositeSamplerState(5, D3DTEXF_LINEAR);
	SetCompositeSamplerState(6, D3DTEXF_LINEAR);

	device->SetTexture(0, textures->gmodTextures.depth.Get());
	device->SetTexture(1, textures->gmodTextures.normal.Get());
	device->SetTexture(2, textures->gmodTextures.position.Get());
	device->SetTexture(3, backBuffer.Get());
	device->SetTexture(4, textures->gmodTextures.thickness.Get());
	device->SetTexture(5, GetCubemap());
	device->SetTexture(6, textures->gmodTextures.albedo.Get());

	device->SetStreamSource(0, ndcQuad.Get(), 0, sizeof(NDCVertex));
	device->SetFVF(D3DFVF_XYZW | D3DFVF_TEX1);

	device->SetRenderState(D3DRS_ZENABLE, TRUE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// Ensures that any left over decal rendering doesn't interfere with the
	// composite
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SRGBWRITEENABLE, TRUE);
	// and multisanple antialiasing
	device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	stateBlock->Apply();

	// Then we composite foam again so that the foam's alpha blend includes the
	// composite

	CompositeFoam(true);
}

void StandardPipeline::Render() {
	UpdateGellyRenderParams();
	RenderGellyFrame();
}