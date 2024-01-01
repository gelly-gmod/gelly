#ifndef CD3D11MAPPEDBUFFERVIEW_H
#define CD3D11MAPPEDBUFFERVIEW_H

#include <d3d11.h>

#include "IMappedBufferView.h"
#include "IRenderContext.h"

class CD3D11MappedBufferView : public IMappedBufferView {
private:
	D3D11_MAPPED_SUBRESOURCE m_resource;
	ID3D11Buffer* m_buffer;
	GellyInterfaceVal<IRenderContext> m_context;

	size_t m_elementSize;
	size_t m_bufferSize;
	void* m_bufferStart;

	bool m_isMapped;
public:
	CD3D11MappedBufferView();

	void AttachToContext(GellyInterfaceVal<IRenderContext> context) override;
	void View(GellyInterfaceRef<IManagedBuffer> buffer) override;

	void* GetBufferStart() override;
	size_t GetBufferSize() override;
	size_t GetElementSize() override;

	~CD3D11MappedBufferView() override;
};

#endif //CD3D11MAPPEDBUFFERVIEW_H
