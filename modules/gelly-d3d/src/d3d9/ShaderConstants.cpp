#include "detail/d3d9/ShaderConstants.h"

#include "ErrorHandling.h"

namespace d3d9 {
void SetConstantVectors(
	IDirect3DDevice9 *device, int reg, float *vector, int count
) {
	DX("Failed to set constant vectors",
	   device->SetVertexShaderConstantF(reg, vector, count));
}

void SetConstantMatrix(
	IDirect3DDevice9 *device, int reg, const D3DMATRIX &matrix
) {
	// D3DMatrix is stored internally as a float[4][4], so we can just cast it
	SetConstantVectors(device, reg, (float *)&matrix, 4);
}

}  // namespace d3d9