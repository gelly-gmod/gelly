#ifndef CD3D11MANAGEDBUFFERLAYOUT_H
#define CD3D11MANAGEDBUFFERLAYOUT_H

#include "IManagedBufferLayout.h"
#include <d3d11.h>

class CD3D11ManagedBufferLayout : public IManagedBufferLayout {
private:
	ID3D11InputLayout *layout;
	GellyObserverPtr<IManagedBuffer> buffers[8]{};
	BufferLayoutDesc desc;
	GellyObserverPtr<IRenderContext> context;

public:
	CD3D11ManagedBufferLayout();
	~CD3D11ManagedBufferLayout() override;

	const BufferLayoutDesc &GetLayoutDesc() override;
	void SetLayoutDesc(const BufferLayoutDesc &desc) override;

	void AttachToContext(GellyObserverPtr<IRenderContext> context) override;

	void Create() override;
	void Destroy() override;

	void AttachBufferAtSlot(
		GellyObserverPtr<IManagedBuffer> buffer, uint8_t slot
	) override;

	void BindAsVertexBuffer() override;
};

#endif	// CD3D11MANAGEDBUFFERLAYOUT_H
