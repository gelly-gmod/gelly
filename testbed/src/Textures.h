#ifndef TEXTURES_H
#define TEXTURES_H

#include <d3d11.h>

#include "ILogger.h"

#define BUILTIN_BACKBUFFER_TEXNAME "backbuffer_tex"

namespace testbed {
/**
 * \brief Sourced textures are textures that are loaded from files.
 */
struct SourcedTextureInfo {
	unsigned char *data;
	unsigned int width;
	unsigned int height;
	DXGI_FORMAT format;
};

/**
 * \brief Feature textures are textures that are generated by the engine.
 */
struct FeatureTextureInfo {
	unsigned int width;
	unsigned int height;
	DXGI_FORMAT format;
};

struct UnownedTextureInfo {
	ID3D11Texture2D *texture;
	ID3D11ShaderResourceView *srv;
	ID3D11RenderTargetView *rtv;
	ID3D11SamplerState *sampler;
};

void InitializeTextureSystem(ILogger *newLogger, ID3D11Device *rendererDevice);

void CreateFeatureTexture(const char *name, const FeatureTextureInfo &info);
void CreateUnownedTexture(const char *name, const UnownedTextureInfo &info);
// TODO: Add source texture creation

// TODO: Switch to actual weak smart pointers

/**
 * \brief
 * \param name Name of the texture
 * \return Retrieves a weak unowned pointer to the texture's render target view.
 */
ID3D11RenderTargetView *GetTextureRTV(const char *name);
/**
 * \brief
 * \param name Name of the texture
 * \return Retrieves a weak unowned pointer to the texture's shader resource
 * view.
 */
ID3D11ShaderResourceView *GetTextureSRV(const char *name);

ID3D11SamplerState *GetTextureSampler(const char *name);

HANDLE GetTextureSharedHandle(const char *name);

}  // namespace testbed

#endif	// TEXTURES_H
