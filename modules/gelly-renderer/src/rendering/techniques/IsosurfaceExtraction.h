#ifndef GELLY_ISOSURFACEEXTRACTION_H
#define GELLY_ISOSURFACEEXTRACTION_H

#include <GellyD3D.h>
#include <d3d11.h>

#include "rendering/Technique.h"

using IntBuffer = d3d11::Buffer<int>;
using IntSRV = d3d11::SRVBuffer<int>;
struct Position {
	float x;
	float y;
	float z;
	float w;
};
using PositionSRV = d3d11::SRVBuffer<Position>;

class IsosurfaceExtraction : public Technique {
private:
	d3d11::ComputeProgram extractionProgram;
	IntBuffer neighborBuffer;
	IntBuffer neighborCountBuffer;
	IntBuffer internalToAPIBuffer;
	IntBuffer APIToInternalBuffer;
	IntSRV neighborSRV;
	IntSRV internalToAPISRV;
	IntSRV APIToInternalSRV;
	IntSRV neighborCountSRV;
	PositionSRV positionSRV;
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
	[[nodiscard]] ID3D11Buffer *GetInternalToAPIBuffer() const;
	[[nodiscard]] ID3D11Buffer *GetAPIToInternalBuffer() const;
	[[nodiscard]] ID3D11Buffer *GetNeighborCountBuffer() const;
};
#endif	// GELLY_ISOSURFACEEXTRACTION_H
