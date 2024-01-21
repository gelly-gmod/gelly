#include "CRaylibVisualDebugFacility.h"

static Color FloatRGBToColor(float r, float g, float b) {
	uint8_t red = static_cast<uint8_t>(floorf(r * 255.f));
	uint8_t green = static_cast<uint8_t>(floorf(g * 255.f));
	uint8_t blue = static_cast<uint8_t>(floorf(b * 255.f));

	return Color{red, green, blue, 255};
}

void CRaylibVisualDebugFacility::Draw3DLine(
	const float *start, const float *end, float r, float g, float b
) {
	DrawLine3D(
		Vector3{start[0], start[1], start[2]},
		Vector3{end[0], end[1], end[2]},
		FloatRGBToColor(r, g, b)
	);
}

void CRaylibVisualDebugFacility::Draw3DCube(
	const float *position, const float *size, float r, float g, float b
) {
	DrawCubeV(
		Vector3{position[0], position[1], position[2]},
		Vector3{size[0], size[1], size[2]},
		FloatRGBToColor(r, g, b)
	);
}

void CRaylibVisualDebugFacility::Draw3DPoint(
	const float *point, float r, float g, float b
) {
	DrawPoint3D(
		Vector3{point[0], point[1], point[2]}, FloatRGBToColor(r, g, b)
	);
}

void CRaylibVisualDebugFacility::Draw3DTriangle(
	const float *A, const float *B, const float *C, float r, float g, float b
) {
	DrawTriangle3D(
		Vector3{A[0], A[1], A[2]},
		Vector3{B[0], B[1], B[2]},
		Vector3{C[0], C[1], C[2]},
		FloatRGBToColor(r, g, b)
	);
}
