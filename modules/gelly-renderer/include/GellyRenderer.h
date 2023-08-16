#ifndef GELLY_GELLYRENDERER_H
#define GELLY_GELLYRENDERER_H

#include <windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <Shader.h>

using namespace Microsoft::WRL;

struct Vec4 {
    float x, y, z, w;
};

struct RendererInitParams {
    int maxParticles;
    int width;
    int height;
    HANDLE* inputNormalSharedHandle;
};

class RendererResources {
private:
    ComPtr<ID3D11Buffer> particles;
    struct {
        ComPtr<ID3D11Texture2D> normal;
    } gbuffer;

    struct {
        PixelShader particleSplat;
    } pixelShaders;

    struct {
        VertexShader particleSplat;
    } vertexShaders;
public:
    RendererResources(ID3D11Device* device, const RendererInitParams& params);
    ~RendererResources() = default;
};

class GellyRenderer {
private:
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> deviceContext;
    RendererResources* resources;
public:
    /**
     * This retrieves the D3D11 buffer which contains the particle data.
     * @note This is meant for doing GPU-GPU copying with FleX.
     * @return
     */
    ID3D11Buffer* GetD3DParticleBuffer() const;
    void Render();
    GellyRenderer(const RendererInitParams& params);
    ~GellyRenderer();
};

GellyRenderer* GellyRenderer_Create(const RendererInitParams& params);
void GellyRenderer_Destroy(GellyRenderer* renderer);

#endif //GELLY_GELLYRENDERER_H
