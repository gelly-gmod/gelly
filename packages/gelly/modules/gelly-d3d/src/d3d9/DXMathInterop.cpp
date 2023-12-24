#include "detail/d3d9/DXMathInterop.h"

// Basically we're just enforcing type->type conversions
// that we know are going to be safe.

// For example, we can't just cast a D3DMATRIX to a XMMATRIX because of
// alignment issues. However, we can cast a D3DMATRIX to a XMFLOAT4X4 and
// vice-versa since at their core they're just arrays of floats.

namespace d3d9 {
void ConvertMatrix(const D3DMATRIX &matrix, XMFLOAT4X4 *out) {
	(*out) = reinterpret_cast<const XMFLOAT4X4 &>(matrix);
}

void ConvertMatrix(const XMFLOAT4X4 &matrix, D3DMATRIX *out) {
	(*out) = reinterpret_cast<const D3DMATRIX &>(matrix);
}
}  // namespace d3d9