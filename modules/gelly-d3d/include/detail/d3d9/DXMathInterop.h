#ifndef GELLY_DXMATHINTEROP_H
#define GELLY_DXMATHINTEROP_H

// This allows for DirectXMath to be used in conjunction with D3D9.
// It's pretty much only for the highly-efficient and well-implemented matrix
// math functions that DirectXMath provides.

#include <d3d9.h>
#include <d3d9types.h>
#include <directxmath.h>

using namespace DirectX;

namespace d3d9 {
void ConvertMatrix(const D3DMATRIX &matrix, XMFLOAT4X4 *out);
void ConvertMatrix(const XMFLOAT4X4 &matrix, D3DMATRIX *out);
}  // namespace d3d9

#endif	// GELLY_DXMATHINTEROP_H
