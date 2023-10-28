#ifndef GELLY_CD3D11SIMCONTEXT_H
#define GELLY_CD3D11SIMCONTEXT_H

#include <d3d11.h>

#include "ISimContext.h"

class CD3D11SimContext : public ISimContext {
private:
	ID3D11Device *device;
	ID3D11DeviceContext *deviceContext;

public:
	CD3D11SimContext(ID3D11Device *device, ID3D11DeviceContext *deviceContext);
	~CD3D11SimContext() = default;

	void *GetRenderAPIHandle(RenderAPIHandle handle) override;
};

#endif	// GELLY_CD3D11SIMCONTEXT_H
