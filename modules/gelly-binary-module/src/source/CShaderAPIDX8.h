#ifndef GELLY_CSHADERAPIDX8_H
#define GELLY_CSHADERAPIDX8_H

#define SHADER_API_VERSION "ShaderApi030"

#include <d3d9.h>

#include "CTexture.h"

typedef void ***CShaderAPIDX8;

CShaderAPIDX8 GetShaderAPIDX8();

IDirect3DBaseTexture9 *GetD3DTexture(CTexture *texture);

#endif	// GELLY_CSHADERAPIDX8_H
