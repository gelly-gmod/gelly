#ifndef GELLY_TEXTUREOVERRIDE_H
#define GELLY_TEXTUREOVERRIDE_H

// The texture override allows us to retrofit shared textures into the Source
// Engine. We conditionally enable a detour that allows us to override the D3D9
// texture creation parameters. By doing this, we can fool the Source Engine
// into thinking that we're creating a normal texture, but it's actually a
// shared texture.

#include <GellyRenderer.h>
#include <MinHook.h>
#include <d3d9.h>

typedef HRESULT(WINAPI *
					D3DCreateTexture)(IDirect3DDevice9 *, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DTexture9 **, HANDLE *);

enum class TextureOverrideTarget : unsigned short {
	None,
	Normal,
	Depth,
};

struct Textures {
	IDirect3DTexture9 *normal;
	IDirect3DTexture9 *depth;
};

namespace TextureOverride {
extern D3DCreateTexture originalCreateTexture;
extern SharedTextures sharedTextures;
extern Textures textures;
extern TextureOverrideTarget target;
extern IDirect3DDevice9 *device;

/**
 * Initializes MinHook, grabs the D3D9 vtable, and sets up the detour.
 * This function is highly error-prone, so it's best to call it as early as
 * possible. It's also recommended to wrap this in a try-catch block since this
 * function will throw a std::runtime_error if it fails.
 */
void Initialize();

/**
 * Shuts down MinHook and removes the detour.
 */
void Shutdown();

void Enable(TextureOverrideTarget overrideTarget);

void Disable();
}  // namespace TextureOverride

#endif	// GELLY_TEXTUREOVERRIDE_H
