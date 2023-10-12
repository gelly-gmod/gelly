#ifndef GELLY_SOLVERCONTEXT_H
#define GELLY_SOLVERCONTEXT_H

#include <GellyD3D.h>
#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class SolverContext {
private:
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> deviceContext;

public:
	// TODO: implement this
};

#endif	// GELLY_SOLVERCONTEXT_H
