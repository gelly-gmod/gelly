#include <GellyRenderer.h>
#include "ErrorHandling.h"

#define MAKE_GBUFFER_TEXTURE(name, format) \
    D3D11_TEXTURE2D_DESC name##Desc; \
    ZeroMemory(&name##Desc, sizeof(name##Desc)); \
    name##Desc.Width = params.width; \
    name##Desc.Height = params.height; \
    name##Desc.MipLevels = 1; \
    name##Desc.ArraySize = 1; \
    name##Desc.Format = format; \
    name##Desc.SampleDesc.Count = 1; \
    name##Desc.BindFlags = D3D11_BIND_RENDER_TARGET; \
    DX("Failed to make "#name" texture", device->CreateTexture2D(&name##Desc, nullptr, gbuffer.name.GetAddressOf()));

RendererResources::RendererResources(ID3D11Device* device, const RendererInitParams& params) :
    gbuffer({
        .normal = nullptr
    }),
    pixelShaders({
        .particleSplat = PixelShader(device, "shaders/ParticleSplatPS.hlsl", "PSMain", nullptr),
    }),
    vertexShaders({
        .particleSplat = VertexShader(device, "shaders/ParticleSplatVS.hlsl", "VSMain", nullptr)
    })
{
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.ByteWidth = sizeof(Vec4) * params.maxParticles;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    DX("Failed to make point vertex buffer",
       device->CreateBuffer(&bufferDesc, nullptr, particles.GetAddressOf()));

    ID3D11Resource* normalResource = nullptr;
    DX("Failed to open the texture in D3D11 from D3D9!",
       device->OpenSharedResource(*params.inputNormalSharedHandle, __uuidof(ID3D11Resource), (void**)&normalResource));

    DX("Failed to query D3D11Texture2D from the shared resource!",
       normalResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)gbuffer.normal.GetAddressOf()));

    DX("Failed to release the shared resource!",
       normalResource->Release());
}

GellyRenderer::GellyRenderer(const RendererInitParams &params) :
    device(nullptr),
    deviceContext(nullptr),
    resources(nullptr) {
    D3D_FEATURE_LEVEL featureLevel[1] = {D3D_FEATURE_LEVEL_11_0};

    DX("Failed to create the D3D11 device.",
       D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevel, 1, D3D11_SDK_VERSION,
                         device.GetAddressOf(), nullptr, deviceContext.GetAddressOf()));

    resources = new RendererResources(device.Get(), params);
}

GellyRenderer::~GellyRenderer() {
    delete resources;
}

GellyRenderer* GellyRenderer_Create(const RendererInitParams& params) {
    return new GellyRenderer(params);
}

void GellyRenderer_Destroy(GellyRenderer* renderer) {
    delete renderer;
}
