#include <GellyRenderer.h>
#include "detail/ErrorHandling.h"

RendererResources::RendererResources(ID3D11Device *device, const RendererInitParams &params) :
        gbuffer({
                        .normal = nullptr
                }),
        pixelShaders({
                             .particleSplat = PixelShader(device, L"shaders/ParticleSplatPS.hlsl", "PSMain", nullptr)
                     }),
        vertexShaders({
                              .particleSplat = VertexShader(device, L"shaders/ParticleSplatVS.hlsl", "VSMain", nullptr)
                      }),
        particleSplatCBuffer(device) {
    // Quad vertex buffer, we're using the triangle method to draw a quad
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.ByteWidth = sizeof(QuadPoint) * 6;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    auto *initPositions = new QuadPoint[6];
    // 1st triangle
    initPositions[0] = {-1.0f, -1.0f, 0.0f};
    initPositions[1] = {-1.0f, 1.0f, 0.0f};
    initPositions[2] = {1.0f, -1.0f, 0.0f};
    // 2nd triangle
    initPositions[3] = {1.0f, -1.0f, 0.0f};
    initPositions[4] = {-1.0f, 1.0f, 0.0f};
    initPositions[5] = {1.0f, 1.0f, 0.0f};

    initData.pSysMem = initPositions;

    DX("Failed to make point vertex buffer",
       device->CreateBuffer(&bufferDesc, &initData, quad.GetAddressOf()));

    // Point instance buffer
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.ByteWidth = sizeof(ParticlePoint) * params.maxParticles;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    // Random particle data
    auto *particleData = new ParticlePoint[params.maxParticles];
    D3D11_SUBRESOURCE_DATA particleInitData;
    ZeroMemory(&particleInitData, sizeof(particleInitData));

    for (int i = 0; i < params.maxParticles; i++) {
        particleData[i] = {
                (float) rand() / (float) RAND_MAX * 2.f,
                (float) rand() / (float) RAND_MAX * 12.f,
                (float) rand() / (float) RAND_MAX * 23.f,
                0.0f
        };
    }

    particleInitData.pSysMem = particleData;

    DX("Failed to make point instance buffer",
       device->CreateBuffer(&bufferDesc, &particleInitData, particles.GetAddressOf()));
    delete[] particleData;

    particleInputLayout[0] = {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0};
    // Instancing data
    particleInputLayout[1] = {
            "INSTANCEPOS",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            1,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_INSTANCE_DATA,
            1,
    };

    DX("Failed to make input layout.", device->CreateInputLayout(particleInputLayout, 2,
                                                                 vertexShaders.particleSplat.GetShaderBlob()->GetBufferPointer(),
                                                                 vertexShaders.particleSplat.GetShaderBlob()->GetBufferSize(),
                                                                 particleInputLayoutObject.GetAddressOf()));

    ID3D11Resource *normalResource = nullptr;
    DX("Failed to open the texture in D3D11 from D3D9!",
       device->OpenSharedResource(*params.sharedTextures.normal, __uuidof(ID3D11Resource), (void **) &normalResource));

    DX("Failed to query D3D11Texture2D from the shared resource!",
       normalResource->QueryInterface(__uuidof(ID3D11Texture2D), (void **) gbuffer.normal.GetAddressOf()));

    DX("Failed to release the shared resource!",
       normalResource->Release());

    // Create render target views for the gbuffer textures

    D3D11_RENDER_TARGET_VIEW_DESC normalRTVDesc;
    ZeroMemory(&normalRTVDesc, sizeof(normalRTVDesc));

    normalRTVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    normalRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    normalRTVDesc.Texture2D.MipSlice = 0;

    DX("Failed to make normal RTV",
       device->CreateRenderTargetView(gbuffer.normal.Get(), &normalRTVDesc, gbuffer.normalRTV.GetAddressOf()));
}

GellyRenderer::GellyRenderer(const RendererInitParams &params) :
        device(nullptr),
        deviceContext(nullptr),
        resources(nullptr),
        params(params) {
    D3D_FEATURE_LEVEL featureLevel[1] = {D3D_FEATURE_LEVEL_11_1};

    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    DX("Failed to create the D3D11 device.",
       D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, featureLevel, 1, D3D11_SDK_VERSION,
                         device.GetAddressOf(), nullptr, deviceContext.GetAddressOf()));

    resources = new RendererResources(device.Get(), params);

    camera.SetPerspective(0.5, (float) params.width / (float) params.height, 0.01f, 1000.0f);
    camera.SetPosition(0.0f, 0.0f, 0.0f);
    camera.SetRotation(0.0f, 0.0f, 0.0f);

    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
}

void GellyRenderer::Render() {
    // Clear the gbuffer
    float testGreen = sinf((float) GetTickCount() / 1000.0f);
    float testRed = cosf((float) GetTickCount() / 1000.0f);
    float clearColor[4] = {testRed, fabsf(testGreen), 1.0f, 1.0f};
    deviceContext->ClearRenderTargetView(resources->gbuffer.normalRTV.Get(), clearColor);

    // Set the gbuffer as the render target
    ID3D11RenderTargetView *renderTargets[] = {
            resources->gbuffer.normalRTV.Get()
    };
    deviceContext->OMSetRenderTargets(1, renderTargets, nullptr);

    // Set up the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(viewport));
    viewport.Width = (float) params.width;
    viewport.Height = (float) params.height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

    deviceContext->RSSetViewports(1, &viewport);

    // Set up the IA
    UINT strides[2] = {
            sizeof(QuadPoint),
            sizeof(ParticlePoint),
    };
    UINT offsets[2] = {0, 0};
    ID3D11Buffer *buffers[2] = {
            resources->quad.Get(),
            resources->particles.Get()
    };

    deviceContext->IASetVertexBuffers(
            0,
            1,
            &buffers[0],
            &strides[0],
            &offsets[0]
    );

    deviceContext->IASetInputLayout(resources->particleInputLayoutObject.Get());
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set up the shaders
    deviceContext->VSSetShader(resources->vertexShaders.particleSplat.GetShader(), nullptr, 0);
    deviceContext->PSSetShader(resources->pixelShaders.particleSplat.GetShader(), nullptr, 0);

    // Orbit around the origin
    camera.SetRotation(0.0f, (float) GetTickCount() / 1000.0f, 0.0f);
    camera.SetPosition(0.0f, 0.0f, 10.0f);

    // Set up the constant buffer data
    ParticleSplatCBuffer cbufferData{};
    cbufferData.vp = camera.GetVPMatrix();

    resources->particleSplatCBuffer.Set(deviceContext.Get(), &cbufferData);
    resources->particleSplatCBuffer.BindToShaders(deviceContext.Get(), 0);

    // Set up the output merger
    deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    deviceContext->OMSetDepthStencilState(nullptr, 0);

    // Set up the rasterizer
    deviceContext->RSSetState(rasterizerState.Get());

    // Draw the particles
    // MENTAL RECAP: THIS IS DRAWING 1 QUAD FOR EVERY 4 PARTICLES. WHY? IT SHOULD BE DRAWING 1 QUAD FOR EVERY PARTICLE.
    deviceContext->DrawInstanced(6, params.maxParticles, 0, 0);

    // Send our commands to the GPU. We don't have the usual swap-chain configuration, so we're just going to manually make the pipeline run.
    deviceContext->Flush();
}

ID3D11Buffer *GellyRenderer::GetD3DParticleBuffer() const {
    return resources->particles.Get();
}



GellyRenderer::~GellyRenderer() {
    delete resources;
}

#ifdef _DEBUG
void GellyRenderer::PrintDebugMessages() {
    // Lazily initialize the debug message queue
    if (debugMsgQueue == nullptr) {
        DX("Failed to get debug message queue",
           device.Get()->QueryInterface(__uuidof(ID3D11InfoQueue), (void **) debugMsgQueue.GetAddressOf()));
    }

    // Print all the messages
    UINT64 messageCount = debugMsgQueue->GetNumStoredMessages();
    for (UINT64 i = 0; i < messageCount; i++) {
        SIZE_T messageLength = 0;
        DX("Failed to get message length", debugMsgQueue->GetMessage(i, nullptr, &messageLength));
        auto *message = new D3D11_MESSAGE[messageLength];
        DX("Failed to get message", debugMsgQueue->GetMessage(i, message, &messageLength));
        printf("[D3D11 Debug Layer]: %s\n", message->pDescription);
        delete[] message;
    }
}
#endif

GellyRenderer *GellyRenderer_Create(const RendererInitParams &params) {
    return new GellyRenderer(params);
}

void GellyRenderer_Destroy(GellyRenderer *renderer) {
    delete renderer;
}
