#ifndef IVISUALDEBUGFACILITY_H
#define IVISUALDEBUGFACILITY_H

#include <cstdint>

namespace gcr::debugging {
class __declspec(novtable) IVisualDebugFacility {
public:
	virtual ~IVisualDebugFacility() = default;

	virtual void Draw3DLine(
		const float *start, const float *end, float r, float g, float b
	) = 0;

	virtual void Draw3DPoint(const float *point, float r, float g, float b) = 0;

	virtual void Draw3DTriangle(
		float *A, float *B, float *C, float r, float g, float b
	) = 0;

	virtual void Draw3DCube(
		const float *position, const float *size, float r, float g, float b
	) = 0;

	virtual void Draw3DWireCube(
		const float *position, const float *size, float r, float g, float b
	) = 0;
};
}  // namespace gcr::debugging

#endif	// IVISUALDEBUGFACILITY_H
