#include "detail/d3d9/DXMathInterop.h"

// hope the compiler inlines this...

namespace d3d9 {
void ConvertMatrix(const D3DMATRIX &matrix, XMFLOAT4X4 *out) {
	out->m[0][0] = matrix._11;
	out->m[0][1] = matrix._12;
	out->m[0][2] = matrix._13;
	out->m[0][3] = matrix._14;
	out->m[1][0] = matrix._21;
	out->m[1][1] = matrix._22;
	out->m[1][2] = matrix._23;
	out->m[1][3] = matrix._24;
	out->m[2][0] = matrix._31;
	out->m[2][1] = matrix._32;
	out->m[2][2] = matrix._33;
	out->m[2][3] = matrix._34;
	out->m[3][0] = matrix._41;
	out->m[3][1] = matrix._42;
	out->m[3][2] = matrix._43;
	out->m[3][3] = matrix._44;
}

void ConvertMatrix(const XMFLOAT4X4 &matrix, D3DMATRIX *out) {
	out->_11 = matrix.m[0][0];
	out->_12 = matrix.m[0][1];
	out->_13 = matrix.m[0][2];
	out->_14 = matrix.m[0][3];
	out->_21 = matrix.m[1][0];
	out->_22 = matrix.m[1][1];
	out->_23 = matrix.m[1][2];
	out->_24 = matrix.m[1][3];
	out->_31 = matrix.m[2][0];
	out->_32 = matrix.m[2][1];
	out->_33 = matrix.m[2][2];
	out->_34 = matrix.m[2][3];
	out->_41 = matrix.m[3][0];
	out->_42 = matrix.m[3][1];
	out->_43 = matrix.m[3][2];
	out->_44 = matrix.m[3][3];
}
}  // namespace d3d9