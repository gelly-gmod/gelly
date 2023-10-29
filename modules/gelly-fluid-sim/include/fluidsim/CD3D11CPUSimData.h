#ifndef GELLY_CD3D11CPUSIMDATA_H
#define GELLY_CD3D11CPUSIMDATA_H

#include <GellyD3D.h>
#include <d3d11.h>

#include "ISimContext.h"
#include "ISimData.h"

class CD3D11CPUSimData : public ISimData {
private:
	d3d11::Buffer<SimFloat4> positions;
	ID3D11Device *device;
	ID3D11DeviceContext *deviceContext;

public:
	explicit CD3D11CPUSimData(ISimContext *context);
	~CD3D11CPUSimData() override = default;

	void Initialize(int maxParticles) override;

	void *GetRenderBuffer(SimBuffer buffer) override;

	SimFloat4 *MapBuffer(SimBuffer buffer) override;
	void UnmapBuffer(SimBuffer buffer) override;
};

#endif	// GELLY_CD3D11CPUSIMDATA_H
