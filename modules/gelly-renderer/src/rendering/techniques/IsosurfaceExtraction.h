#ifndef GELLY_ISOSURFACEEXTRACTION_H
#define GELLY_ISOSURFACEEXTRACTION_H

#include <GellyD3D.h>
#include <d3d11.h>

#include "rendering/Technique.h"

using IntBuffer = d3d11::Buffer<int>;
using IntUAV = d3d11::UAVBuffer<int>;
struct Position {
	float x;
	float y;
	float z;
	float w;
};
using PositionUAV = d3d11::UAVBuffer<Position>;

class IsosurfaceExtraction : public Technique {
private:
	d3d11::ComputeProgram extractionProgram;
	IntBuffer neighborBuffer;
	IntBuffer remapBuffer;
	IntBuffer neighborCountBuffer;
	IntUAV neighborUAV;
	IntUAV remapUAV;
	IntUAV neighborCountUAV;
	PositionUAV positionUAV;
	d3d11::ComputeProgramLayout<PerFrameCBuffer> layout;

public:
	IsosurfaceExtraction(
		ID3D11Device *device, ID3D11Buffer *particleBuffer, int maxParticles
	);
	~IsosurfaceExtraction() = default;

	void RunForFrame(
		ID3D11DeviceContext *context, TechniqueResources *resources
	) override;

	[[nodiscard]] ID3D11Buffer *GetNeighborBuffer() const;
	[[nodiscard]] ID3D11Buffer *GetRemapBuffer() const;
	[[nodiscard]] ID3D11Buffer *GetNeighborCountBuffer() const;
};
#endif	// GELLY_ISOSURFACEEXTRACTION_H
