#ifndef GELLY_SHADERCONSTANTS_H
#define GELLY_SHADERCONSTANTS_H

#include <d3d9.h>

namespace d3d9 {
void SetConstantVectors(
	IDirect3DDevice9 *device, int reg, float *vector, int count
);

void SetConstantMatrix(
	IDirect3DDevice9 *device, int reg, const D3DMATRIX &matrix
);
}  // namespace d3d9

#endif	// GELLY_SHADERCONSTANTS_H
