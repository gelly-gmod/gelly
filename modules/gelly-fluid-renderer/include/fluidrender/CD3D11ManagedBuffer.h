#ifndef GELLY_CD3D11MANAGEDBUFFER_H
#define GELLY_CD3D11MANAGEDBUFFER_H

#include <d3d11.h>

#include "IManagedBuffer.h"

class CD3D11ManagedBuffer : public IManagedBuffer {
private:
	ID3D11Buffer *buffer;
	ID3D11ShaderResourceView *srv;
	ID3D11UnorderedAccessView *uav;

	BufferDesc desc;
	GellyObserverPtr<IRenderContext> context;

public:
	CD3D11ManagedBuffer();
	~CD3D11ManagedBuffer() override;

	void SetDesc(const BufferDesc &desc) override;
	[[nodiscard]] const BufferDesc &GetDesc() const override;

	bool Create() override;
	void Destroy() override;
	void AttachToContext(IRenderContext *context) override;

	void *GetBufferResource() override;
	
	GellyObserverPtr<ID3D11ShaderResourceView> GetSRV();
	GellyObserverPtr<ID3D11UnorderedAccessView> GetUAV();
};

#endif	// GELLY_CD3D11MANAGEDBUFFER_H
