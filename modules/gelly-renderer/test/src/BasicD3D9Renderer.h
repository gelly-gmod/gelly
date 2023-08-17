#ifndef GELLY_BASICD3D9RENDERER_H
#define GELLY_BASICD3D9RENDERER_H

#include <windows.h>
#include <wrl.h>
#define D3D_DEBUG_INFO
#include <d3d9.h>
#include <SDL.h>

using namespace Microsoft::WRL;

/**
 * This is a basic D3D9 renderer that is used to test the D3D11 renderer.
 * This renderer will have some textures that we're going to try to share to the D3D11 renderer.
 */
class BasicD3D9Renderer {
private:
    ComPtr<IDirect3D9Ex> d3d9;
    ComPtr<IDirect3DDevice9Ex> device;
    struct {
        IDirect3DTexture9* input_normal;
        HANDLE* input_normal_shared_handle = nullptr;
    } gbuffer;
    ComPtr<IDirect3DVertexBuffer9> vertexBuffer;
public:
    explicit BasicD3D9Renderer(SDL_Window* window);
    ~BasicD3D9Renderer() = default;

    void Render();

    [[nodiscard]] HANDLE* GetInputNormalSharedHandle();
};

#endif //GELLY_BASICD3D9RENDERER_H
