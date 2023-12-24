#ifndef TESTSSFX_H
#define TESTSSFX_H

#include "Rendering.h"
#include "SSFX.h"

#define SHADINGSSFX_EFFECT_NAME "ssfx/test"

namespace testbed::ssfx {
struct ShadingCBuffer {
	struct Light {
		float3 position;
		float power;
		float radius;
		float3 color;
	};

	Light lights[2];
	float lightCount;
	float pad0_;
	float pad1_;
	float pad2_;
};

void InitializeShadingSSFX(ILogger *logger);
void UpdateShadingSSFXConstants();
/**
 * \brief Used primarily for keeping the water shading and opaque shading in
 * sync
 * \return Returns a copy of the shading SSFX effect's constant buffer.
 */
ShadingCBuffer GetShadingSSFXCBuffer();
}  // namespace testbed::ssfx

#endif	// TESTSSFX_H
