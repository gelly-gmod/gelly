#ifndef GELLY_NORMALESTIMATION_H
#define GELLY_NORMALESTIMATION_H

#include <GellyD3D.h>
#include <d3d11.h>
#include <wrl.h>

#include "rendering/SSTechnique.h"

using namespace Microsoft::WRL;

class NormalEstimation : SSTechnique {
private:
	ComPtr<ID3D11PixelShader> pixelShader;

public:
	explicit NormalEstimation(ID3D11Device *device);
	~NormalEstimation() override = default;

	void RunForFrame(
		ID3D11DeviceContext *context, TechniqueResources *resources
	) override;
};

#endif	// GELLY_NORMALESTIMATION_H
