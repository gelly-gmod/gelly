#ifndef GELLY_OUTPUTENCODER_H
#define GELLY_OUTPUTENCODER_H

#include <GellyD3D.h>
#include <d3d11.h>
#include <wrl.h>

#include "rendering/SSTechnique.h"

using namespace Microsoft::WRL;

class OutputEncoder : SSTechnique {
private:
	ComPtr<ID3D11PixelShader> pixelShader;

public:
	explicit OutputEncoder(ID3D11Device *device);
	~OutputEncoder() = default;

	void RunForFrame(
		ID3D11DeviceContext *context, TechniqueResources *resources
	) override;
};

#endif	// GELLY_OUTPUTENCODER_H
