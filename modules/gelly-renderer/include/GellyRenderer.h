#ifndef GELLY_GELLYRENDERER_H
#define GELLY_GELLYRENDERER_H

#include <windows.h>
#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>
#include "detail/Shader.h"
#include "detail/ConstantBuffer.h"
#include "detail/Camera.h"

using namespace Microsoft::WRL;
using namespace DirectX;

// Usually you could use XMVECTOR, but there's no guarantees from FleX about the alignment of the data. It's safer to just use a struct.
struct ParticlePoint {
    float x, y, z, w;
};

struct QuadPoint {
    float x, y, z;
};

struct SharedTextures {
    HANDLE* normal;
};

struct RendererInitParams {
    int maxParticles;
    int width;
    int height;
    SharedTextures sharedTextures;
};

struct ParticleSplatCBuffer {
    XMFLOAT4X4 vp;
};

class RendererResources {
public:
    ComPtr<ID3D11Buffer> quad;
    ComPtr<ID3D11Buffer> particles;

    D3D11_INPUT_ELEMENT_DESC particleInputLayout[2]{};
    ComPtr<ID3D11InputLayout> particleInputLayoutObject;

    ConstantBuffer<ParticleSplatCBuffer> particleSplatCBuffer;

    struct {
        ComPtr<ID3D11Texture2D> normal;
        ComPtr<ID3D11RenderTargetView> normalRTV;
    } gbuffer;

    struct {
        PixelShader particleSplat;
    } pixelShaders;
    struct {
        VertexShader particleSplat;
    } vertexShaders;

    RendererResources(ID3D11Device* device, const RendererInitParams& params);
    ~RendererResources() = default;
};

class GellyRenderer {
private:
#ifdef _DEBUG
    ComPtr<ID3D11InfoQueue> debugMsgQueue;
#endif
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> deviceContext;
    ComPtr<ID3D11RasterizerState> rasterizerState;

    RendererResources* resources;
    Camera camera;
    RendererInitParams params;
public:
    /**
     * This retrieves the D3D11 buffer which contains the particle data.
     * @note This is meant for doing GPU-GPU copying with FleX.
     * @return
     */
    [[nodiscard]] ID3D11Buffer* GetD3DParticleBuffer() const;
    void Render();

#ifdef _DEBUG
    void PrintDebugMessages();
#endif

    explicit GellyRenderer(const RendererInitParams& params);
    ~GellyRenderer();
};

GellyRenderer* GellyRenderer_Create(const RendererInitParams& params);
void GellyRenderer_Destroy(GellyRenderer* renderer);

#endif //GELLY_GELLYRENDERER_H
