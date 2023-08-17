#include <GellyRenderer.h>
#include "detail/ErrorHandling.h"

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
        .particleSplat = PixelShader(device, L"shaders/ParticleSplatPS.hlsl", "PSMain", nullptr)
    }),
    vertexShaders({
        .particleSplat = VertexShader(device, L"shaders/ParticleSplatVS.hlsl", "VSMain", nullptr)
    }),
    particleSplatCBuffer(device)
{
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.ByteWidth = sizeof(Vertex) * params.maxParticles;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    // Make some mock data
    D3D11_SUBRESOURCE_DATA subresourceData;
    ZeroMemory(&subresourceData, sizeof(subresourceData));

    auto* vertices = new Vertex[params.maxParticles];
    for (int i = 0; i < params.maxParticles; i++) {
        vertices[i] = {1.0f, 0.0f, 0.0f, 1.0f};
    }
    subresourceData.pSysMem = vertices;
    subresourceData.SysMemPitch = 0;

    DX("Failed to make point vertex buffer",
       device->CreateBuffer(&bufferDesc, &subresourceData, particles.GetAddressOf()));

    particleInputLayout[0] = {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0};
    device->CreateInputLayout(particleInputLayout, 1, vertexShaders.particleSplat.GetShaderBlob()->GetBufferPointer(),
                              vertexShaders.particleSplat.GetShaderBlob()->GetBufferSize(), particleInputLayoutObject.GetAddressOf());

    ID3D11Resource* normalResource = nullptr;
    DX("Failed to open the texture in D3D11 from D3D9!",
       device->OpenSharedResource(*params.inputNormalSharedHandle, __uuidof(ID3D11Resource), (void**)&normalResource));

    DX("Failed to query D3D11Texture2D from the shared resource!",
       normalResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)gbuffer.normal.GetAddressOf()));

    DX("Failed to release the shared resource!",
       normalResource->Release());

    // Create render target views for the gbuffer textures

    D3D11_RENDER_TARGET_VIEW_DESC normalRTVDesc;
    ZeroMemory(&normalRTVDesc, sizeof(normalRTVDesc));

    normalRTVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    normalRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    normalRTVDesc.Texture2D.MipSlice = 0;

    DX("Failed to make normal RTV", device->CreateRenderTargetView(gbuffer.normal.Get(), &normalRTVDesc, gbuffer.normalRTV.GetAddressOf()));
}

GellyRenderer::GellyRenderer(const RendererInitParams &params) :
    device(nullptr),
    deviceContext(nullptr),
    resources(nullptr),
    params(params) {
    D3D_FEATURE_LEVEL featureLevel[1] = {D3D_FEATURE_LEVEL_11_0};

    DX("Failed to create the D3D11 device.",
       D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevel, 1, D3D11_SDK_VERSION,
                         device.GetAddressOf(), nullptr, deviceContext.GetAddressOf()));

    resources = new RendererResources(device.Get(), params);

    camera.SetPerspective(1, (float)params.width / (float)params.height, 0.1f, 1000.0f);
    camera.SetPosition(0.0f, 0.0f, 0.0f);
    camera.SetRotation(0.0f, 0.0f, 0.0f);
}

void GellyRenderer::Render() {
    // Clear the gbuffer
    float clearColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    deviceContext->ClearRenderTargetView(resources->gbuffer.normalRTV.Get(), clearColor);

    // Set the gbuffer as the render target
    ID3D11RenderTargetView* renderTargets[] = {
        resources->gbuffer.normalRTV.Get()
    };
    deviceContext->OMSetRenderTargets(1, renderTargets, nullptr);

    // Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(viewport));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = params.width;
    viewport.Height = params.height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    deviceContext->RSSetViewports(1, &viewport);

    // Set up the IA
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(
            0,
            1,
            resources->particles.GetAddressOf(),
            &stride,
            &offset
        );

    deviceContext->IASetInputLayout(resources->particleInputLayoutObject.Get());
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    // Set up the shaders
    deviceContext->VSSetShader(resources->vertexShaders.particleSplat.GetShader(), nullptr, 0);
    deviceContext->PSSetShader(resources->pixelShaders.particleSplat.GetShader(), nullptr, 0);

    // Set up the constant buffer data
    ParticleSplatCBuffer cbufferData{};
    cbufferData.view = camera.GetView();
    cbufferData.projection = camera.GetProjection();

    resources->particleSplatCBuffer.Set(deviceContext.Get(), &cbufferData);
    resources->particleSplatCBuffer.BindToShaders(deviceContext.Get(), 0);

    // Draw the particles
    deviceContext->Draw(params.maxParticles, 0);

    // Send our commands to the GPU. We don't have the usual swap-chain configuration, so we're just going to manually make the pipeline run.
    deviceContext->Flush();
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
