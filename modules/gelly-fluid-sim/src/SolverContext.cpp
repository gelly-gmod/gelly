#include "detail/SolverContext.h"

#include <GellyD3D.h>

SolverContext::SolverContext(
	ID3D11Device *deviceReference, ID3D11DeviceContext *deviceContextReference
)
	: device(deviceReference), deviceContext(deviceContextReference) {}

ID3D11Device *SolverContext::GetDevice() const { return device; }

ID3D11DeviceContext *SolverContext::GetDeviceContext() const {
	return deviceContext;
}

d3d11::ComputeProgram SolverContext::CreateComputeProgram(
	const char *programName, const char *entryPoint, const char *programSource
) const {
	return {device, programName, entryPoint, programSource};
}