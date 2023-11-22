#ifndef GELLY_CD3D11CPUSIMDATA_H
#define GELLY_CD3D11CPUSIMDATA_H

#include <GellyD3D.h>
#include <GellyObserverPtr.h>
#include <d3d11.h>

#include "ISimContext.h"
#include "ISimData.h"

class CD3D11CPUSimData : public ISimData {
private:
	ID3D11Buffer *positionBuffer;
	ID3D11Buffer *velocityBuffer;

public:
	explicit CD3D11CPUSimData();
	~CD3D11CPUSimData() override = default;

	void LinkBuffer(SimBufferType type, void *buffer) override;
	bool IsBufferLinked(SimBufferType type) override;

	void *GetLinkedBuffer(SimBufferType type) override;
};

#endif	// GELLY_CD3D11CPUSIMDATA_H
