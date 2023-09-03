#ifndef GELLY_NORMALSMOOTHING_H
#define GELLY_NORMALSMOOTHING_H

#include "PerFrameCBuffer.h"
#include "detail/ConstantBuffer.h"
#include "rendering/SSTechnique.h"

class NormalSmoothing : public SSTechnique {
private:
	ComPtr<ID3D11PixelShader> pixelShader;
	ConstantBuffer<PerFrameCBuffer> perFrameCBuffer;

	ComPtr<ID3D11ShaderResourceView> depthSRV;
	ComPtr<ID3D11SamplerState> depthSampler;

public:
	NormalSmoothing(
		ID3D11Device *device,
		int depthWidth,
		int depthHeight,
		ID3D11Resource *depth
	);
	~NormalSmoothing() override = default;

	void RunForFrame(
		ID3D11DeviceContext *context, TechniqueRTs *rts, const Camera &camera
	) override;
};

#endif	// GELLY_NORMALSMOOTHING_H
