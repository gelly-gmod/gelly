#ifndef GELLY_D3DDEVICEWRAPPER_H
#define GELLY_D3DDEVICEWRAPPER_H

#include <d3d9.h>

// Last known definition of D3DDeviceWrapper.
// Source:
// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/src_main/materialsystem/shaderapidx9/d3d_async.h#L123
// For us, we just want to grab the pointer to the IDirect3DDevice9, so we can
// do that by just defining the class member we need.
class D3DDeviceWrapper {
public:
	IDirect3DDevice9 *m_pD3DDevice;
};

// D3DDevice's location is at BaseShaderAPIDLL + 0x9A9E0
// It's a global class that contains a pointer to the IDirect3DDevice9

IDirect3DDevice9Ex *GetD3DDevice();

#endif	// GELLY_D3DDEVICEWRAPPER_H
