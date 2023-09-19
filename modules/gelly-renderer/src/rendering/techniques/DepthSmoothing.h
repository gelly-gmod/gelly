#ifndef GELLY_DEPTHSMOOTHING_H
#define GELLY_DEPTHSMOOTHING_H
#include <GellyD3D.h>
#include <d3d11.h>
#include <wrl.h>

#include "rendering/SSTechnique.h"

using namespace Microsoft::WRL;

class DepthSmoothing : SSTechnique {
private:
	ComPtr<ID3D11PixelShader> pixelShader;

public:
	explicit DepthSmoothing(ID3D11Device *device);
	~DepthSmoothing() = default;

	void RunForFrame(
		ID3D11DeviceContext *context, TechniqueResources *resources
	) override;
};

#endif	// GELLY_DEPTHSMOOTHING_H
