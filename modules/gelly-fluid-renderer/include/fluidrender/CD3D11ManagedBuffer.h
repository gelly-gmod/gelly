#ifndef GELLY_CD3D11MANAGEDBUFFER_H
#define GELLY_CD3D11MANAGEDBUFFER_H

#include <d3d11.h>

#include <optional>

#include "IManagedBuffer.h"

class CD3D11ManagedBuffer : public IManagedBuffer {
private:
	ID3D11Buffer *buffer;
	ID3D11ShaderResourceView *srv;
	ID3D11UnorderedAccessView *uav;
	std::optional<ID3D11InputLayout *> inputLayout;

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

	void BindToPipeline(ShaderType shaderType, uint8_t slot) override;

	GellyObserverPtr<ID3D11ShaderResourceView> GetSRV() const;
	GellyObserverPtr<ID3D11UnorderedAccessView> GetUAV() const;
};

#endif	// GELLY_CD3D11MANAGEDBUFFER_H
