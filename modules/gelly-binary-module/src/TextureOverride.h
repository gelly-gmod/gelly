#ifndef GELLY_TEXTUREOVERRIDE_H
#define GELLY_TEXTUREOVERRIDE_H

// The texture override allows us to retrofit shared textures into the Source
// Engine. We conditionally enable a detour that allows us to override the D3D9
// texture creation parameters. By doing this, we can fool the Source Engine
// into thinking that we're creating a normal texture, but it's actually a
// shared texture.

// Also, we capture the cubemap textures that are created by the engine and the
// current one used.

#include <GellyD3D.h>
#include <GellyRenderer.h>
#include <MinHook.h>
#include <d3d9.h>

#include "hooking/VTable.h"

/**
 * Creates the detours required for the system.
 */
void TextureOverride_Init();
/**
 * Removes the detours required for the system, and cleans up any resources.
 * This is highly important, as the detours will cause crashes if they're not
 * properly removed.
 */
void TextureOverride_Shutdown();

/**
 * Gets the texture that is currently being created and fills it into the
 * provided pointer. This is *not* asynchronous, but the results also aren't
 * available until the next texture creation. Usually this happens after you
 * make a call to GetRenderTarget, and the provided pointer becomes valid.
 * @param texture
 * @param format Requested format. This won't always be obliged because you're
 * limited to 3 formats. See the note.
 * @note From the Microsoft documentation: "Only R10G10B10A2_UNORM,
 * R16G16B16A16_FLOAT and R8G8B8A8_UNORM formats are allowed."
 */
void TextureOverride_GetTexture(d3d9::Texture **texture, D3DFORMAT format);

/**
 * Returns the current cubemap texture that is being rendered.
 * @note Can be null.
 */
IDirect3DBaseTexture9 *TextureOverride_GetCubemapTexture();

/**
 * Toggles whether or not we should be finding cubemap textures. This is
 * crucial if you're using the cubemap texture for anything, as it will
 * trigger the texture override to find the cubemap texture, causing a loop.
 * @param enable
 */
void TextureOverride_ToggleCubemapFinding(bool enable);

#endif	// GELLY_TEXTUREOVERRIDE_H
