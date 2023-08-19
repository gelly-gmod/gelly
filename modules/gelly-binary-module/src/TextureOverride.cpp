#include <stdexcept>
#include "TextureOverride.h"
#include <d3d9.h>

using D3D9VTable = void*[119];

/**
 * Gets the window handle of the Garry's Mod window.
 * @note This function uses the special window class name "Valve001" to find the Garry's Mod window.
 * @note If another window has the same window class name, there is no guarantee that this function will return the correct window handle.
 * @return
 */
static HWND GetGModWindow() {
    return FindWindowA("Valve001", nullptr);
}

/**
 * Gets the IDirect3DDevice9 vtable. Can fail, so check the return value. If true, the vtable is written to destinationVTable.
 * @see https://bananamafia.dev/post/d3dhook/
 * @param destinationVTable
 * @return
 */
static bool GetD3D9VTable(D3D9VTable destinationVTable) {
    IDirect3D9 *sdk = Direct3DCreate9(D3D_SDK_VERSION);
    if (sdk == nullptr) {
        return false;
    }

    IDirect3DDevice9 *device = nullptr;
    D3DPRESENT_PARAMETERS params = {0};
    params.Windowed = true;
    params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params.hDeviceWindow = GetGModWindow();

    HRESULT hr = sdk->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, params.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &device);
    if (device == nullptr) {
        sdk->Release();
        char error[512];
        sprintf_s(error, "Failed to create the D3D9 device. Error code: 0x%lx\nHWND: %d", hr, params.hDeviceWindow);
        throw std::runtime_error(error);
    }

    // We're going to release this device, so we'll copy the VTable instead of just writing it.
    memcpy(destinationVTable, *reinterpret_cast<void***>(device), sizeof(D3D9VTable));

    device->Release();
    sdk->Release();
    return true;
}

static HRESULT WINAPI HookedD3DCreateTexture(IDirect3DDevice9 *device, UINT width, UINT height, UINT levels, DWORD usage, D3DFORMAT format, D3DPOOL pool, IDirect3DTexture9 **texture, HANDLE *sharedHandle) {
    using namespace TextureOverride;

    if (target == TextureOverrideTarget::None) {
        // This shouldn't happen as the hook should be disabled.
        return originalCreateTexture(device, width, height, levels, usage, format, pool, texture, sharedHandle);
    }

    HANDLE shared_handle = nullptr;
    HANDLE** target_handle_ptr = nullptr;

    D3DFORMAT texFormat = format;
    switch (target) {
        case TextureOverrideTarget::Normal:
            target_handle_ptr = &sharedTextures.normal;
            texFormat = D3DFMT_A16B16G16R16F;
            break;
        default:
            break;
    }

    HRESULT result = originalCreateTexture(device, width, height, 1, D3DUSAGE_RENDERTARGET, texFormat, D3DPOOL_DEFAULT, texture, &shared_handle);
    if (FAILED(result)) {
        return result;
    }

    *target_handle_ptr = new HANDLE(shared_handle);
    return result;
}

static D3DCreateTexture createTexturePtr = nullptr;

namespace TextureOverride {
    D3DCreateTexture originalCreateTexture = nullptr;
    SharedTextures sharedTextures{};
    TextureOverrideTarget target = TextureOverrideTarget::None;
}

void TextureOverride::Initialize() {
    if (MH_Initialize() != MH_OK) {
        throw std::runtime_error("Failed to initialize MinHook.");
    }

    D3D9VTable d3d9VTable{};
    if (!GetD3D9VTable(d3d9VTable)) {
        throw std::runtime_error("Failed to get the D3D9 vtable.");
    }

    // We're going to hook IDirect3DDevice9::CreateTexture
    // This function is at index 23 in the vtable.
    createTexturePtr = reinterpret_cast<D3DCreateTexture>(d3d9VTable[23]);

    if (MH_CreateHook(createTexturePtr, &HookedD3DCreateTexture, reinterpret_cast<void**>(&originalCreateTexture)) != MH_OK) {
        throw std::runtime_error("Failed to create the CreateTexture hook.");
    }
}

void TextureOverride::Shutdown() {
    MH_DisableHook(createTexturePtr);
    MH_Uninitialize();
}

void TextureOverride::Enable(TextureOverrideTarget overrideTarget) {
    target = overrideTarget;
    MH_EnableHook(createTexturePtr);
}

void TextureOverride::Disable() {
    target = TextureOverrideTarget::None;
    MH_DisableHook(createTexturePtr);
}