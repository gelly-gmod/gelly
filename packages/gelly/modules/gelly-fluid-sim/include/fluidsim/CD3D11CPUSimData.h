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
	ID3D11Buffer *anisotropyQ1Buffer;
	ID3D11Buffer *anisotropyQ2Buffer;
	ID3D11Buffer *anisotropyQ3Buffer;

	int maxParticles = 0;
	int activeParticles = 0;

public:
	explicit CD3D11CPUSimData();
	~CD3D11CPUSimData() override = default;

	void LinkBuffer(SimBufferType type, void *buffer) override;
	bool IsBufferLinked(SimBufferType type) override;

	void *GetLinkedBuffer(SimBufferType type) override;
	SimContextAPI GetAPI() override;

	void SetMaxParticles(int maxParticles) override;
	int GetMaxParticles() override;

	void SetActiveParticles(int activeParticles) override;
	int GetActiveParticles() override;
};

#endif	// GELLY_CD3D11CPUSIMDATA_H
