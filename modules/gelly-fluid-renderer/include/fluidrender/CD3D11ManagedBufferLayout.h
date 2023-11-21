#ifndef CD3D11MANAGEDBUFFERLAYOUT_H
#define CD3D11MANAGEDBUFFERLAYOUT_H

#include "IManagedBufferLayout.h"

class CD3D11ManagedBufferLayout : public IManagedBufferLayout {
private:
	ID3D11InputLayout *layout;
	GellyObserverPtr<IManagedBuffer> buffers[8]{};
	Gelly::BufferLayoutDesc desc;
	GellyObserverPtr<IRenderContext> context;

public:
	CD3D11ManagedBufferLayout();
	~CD3D11ManagedBufferLayout() override;

	const Gelly::BufferLayoutDesc &GetLayoutDesc() override;
	void SetLayoutDesc(const Gelly::BufferLayoutDesc &desc) override;

	void AttachToContext(GellyObserverPtr<IRenderContext> context) override;

	void Create() override;
	void Destroy() override;

	void AttachBufferAtSlot(
		GellyObserverPtr<IManagedBuffer> buffer, uint8_t slot
	) override;

	void BindAsVertexBuffer() override;
};

#endif	// CD3D11MANAGEDBUFFERLAYOUT_H
