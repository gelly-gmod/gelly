#include "fluidrender/CD3D11MappedBufferView.h"

CD3D11MappedBufferView::CD3D11MappedBufferView()
	: m_context(nullptr),
	  m_resource({}),
	  m_buffer(nullptr),
	  m_elementSize(0),
	  m_bufferSize(0),
	  m_bufferStart(nullptr),
	  m_isMapped(false) {}

void CD3D11MappedBufferView::AttachToContext(
	GellyInterfaceVal<IRenderContext> context
) {
	m_context = context;
}

void CD3D11MappedBufferView::View(GellyInterfaceRef<IManagedBuffer> buffer) {
	const auto &desc = buffer->GetDesc();
	if (desc.usage != BufferUsage::DYNAMIC) {
		throw std::runtime_error(
			"CD3D11MappedBufferView::View: buffer must be dynamic"
		);
	}

	auto *bufferResource =
		static_cast<ID3D11Buffer *>(buffer->GetBufferResource());
	auto *context = static_cast<ID3D11DeviceContext *>(
		m_context->GetRenderAPIResource(RenderAPIResource::D3D11DeviceContext)
	);

	if (const auto result = context->Map(
			bufferResource, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &m_resource
		);
		FAILED(result)) {
		throw std::runtime_error(
			"CD3D11MappedBufferView::View: failed to map buffer"
		);
	}

	m_buffer = bufferResource;
	m_bufferSize = desc.byteWidth;
	m_elementSize = desc.stride;
	m_bufferStart = m_resource.pData;
	m_isMapped = true;
}

size_t CD3D11MappedBufferView::GetBufferSize() { return m_bufferSize; }
size_t CD3D11MappedBufferView::GetElementSize() { return m_elementSize; }
void *CD3D11MappedBufferView::GetBufferStart() { return m_bufferStart; }

CD3D11MappedBufferView::~CD3D11MappedBufferView() {
	if (m_isMapped) {
		auto *context =
			static_cast<ID3D11DeviceContext *>(m_context->GetRenderAPIResource(
				RenderAPIResource::D3D11DeviceContext
			));

		context->Unmap(m_buffer, 0);
	}
}
