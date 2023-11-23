#ifndef GELLY_CAMERA_H
#define GELLY_CAMERA_H

#include <DirectXMath.h>

#include "Memory.h"
#include "Rendering.h"

namespace testbed {
void InitializeCamera(ILogger *newLogger);
const Camera &GetCamera();
void UpdateCamera();
}  // namespace testbed

#endif	// GELLY_CAMERA_H
