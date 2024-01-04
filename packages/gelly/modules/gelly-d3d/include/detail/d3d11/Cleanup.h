#ifndef GELLY_D3D11CLEANUP_H
#define GELLY_D3D11CLEANUP_H

#include <d3d11.h>

namespace d3d11 {
void CleanupRTsAndShaders(
	ID3D11DeviceContext *context, int usedViews, int usedSamplers
);
}  // namespace splatting

#endif	// GELLY_D3D11CLEANUP_H