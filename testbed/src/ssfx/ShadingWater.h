#ifndef SHADINGWATER_H
#define SHADINGWATER_H

#include "Rendering.h"
#include "SSFX.h"

#define SHADINGWATERSSFX_EFFECT_NAME "ssfx/shadingwater"

namespace testbed::ssfx {
void InitializeShadingWaterSSFX(ILogger *logger);
void UpdateShadingWaterSSFXConstants();
}  // namespace testbed::ssfx

#endif	// SHADINGWATER_H
