#ifndef GELLY_ISOSURFACEEXTRACTION_H
#define GELLY_ISOSURFACEEXTRACTION_H

#include <GellyD3D.h>
#include <d3d11.h>

#include "rendering/Technique.h"

class IsosurfaceExtraction : public Technique {
private:
	d3d11::ComputeProgram extractionProgram;
};
#endif	// GELLY_ISOSURFACEEXTRACTION_H
