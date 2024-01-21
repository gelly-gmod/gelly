#ifndef CRAYLIBVISUALDEBUGFACILITY_H
#define CRAYLIBVISUALDEBUGFACILITY_H

#include <gelly-cpu-refs/debugging/IVisualDebugFacility.h>
#include <raylib.h>

#include <cmath>

class CRaylibVisualDebugFacility : public gcr::debugging::IVisualDebugFacility {
public:
	CRaylibVisualDebugFacility() = default;
	~CRaylibVisualDebugFacility() override = default;

	void Draw3DLine(
		const float *start, const float *end, float r, float g, float b
	) override;
	void Draw3DPoint(const float *point, float r, float g, float b) override;
	void Draw3DTriangle(
		const float *A,
		const float *B,
		const float *C,
		float r,
		float g,
		float b
	) override;
	void Draw3DCube(
		const float *position, const float *size, float r, float g, float b
	) override;
};

#endif	// CRAYLIBVISUALDEBUGFACILITY_H
