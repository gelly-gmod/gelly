#include "fluidrender/CD3D11ManagedBuffer.h"

#include <GellyD3D.h>

#include <stdexcept>

CD3D11ManagedBuffer::CD3D11ManagedBuffer()
	: buffer(nullptr), srv(nullptr), uav(nullptr), context(nullptr), desc({}) {}

CD3D11ManagedBuffer::~CD3D11ManagedBuffer() { CD3D11ManagedBuffer::Destroy(); }

void CD3D11ManagedBuffer::SetDesc(const Gelly::BufferDesc &desc) {
	this->desc = desc;
}

const Gelly::BufferDesc &CD3D11ManagedBuffer::GetDesc() const { return desc; }

bool CD3D11ManagedBuffer::Create() {
	D3D11_BUFFER_DESC d3dBufferDesc = {};
	d3dBufferDesc.ByteWidth = desc.byteWidth;

	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
	switch (desc.usage) {
		case BufferUsage::DEFAULT:
			usage = D3D11_USAGE_DEFAULT;
			break;
		case BufferUsage::DYNAMIC:
			usage = D3D11_USAGE_DYNAMIC;
			break;
	}

	d3dBufferDesc.Usage = usage;

	unsigned long bindFlags = 0;
	if ((desc.type & BufferType::VERTEX) != 0) {
		bindFlags |= D3D11_BIND_VERTEX_BUFFER;
	}

	if ((desc.type & BufferType::CONSTANT) != 0) {
		bindFlags |= D3D11_BIND_CONSTANT_BUFFER;
	}

	if ((desc.type & BufferType::SHADER_RESOURCE) != 0) {
		bindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}

	if ((desc.type & BufferType::UNORDERED_ACCESS) != 0) {
		bindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}

	d3dBufferDesc.BindFlags = bindFlags;

	if (usage == D3D11_USAGE_DYNAMIC) {
		d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	D3D11_SUBRESOURCE_DATA d3dSubresourceData = {};
	d3dSubresourceData.pSysMem = desc.initialData;

	auto *device = static_cast<ID3D11Device *>(
		context->GetRenderAPIResource(RenderAPIResource::D3D11Device)
	);

	DX("Failed to create D3D11 buffer",
	   device->CreateBuffer(&d3dBufferDesc, &d3dSubresourceData, &buffer));

	if ((desc.type & BufferType::SHADER_RESOURCE) != 0) {
		D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc = {};
		d3dSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		d3dSRVDesc.Buffer.FirstElement = 0;
		d3dSRVDesc.Buffer.NumElements = desc.byteWidth / desc.stride;

		DX("Failed to create D3D11 shader resource view",
		   device->CreateShaderResourceView(buffer, &d3dSRVDesc, &srv));
	}

	if ((desc.type & BufferType::UNORDERED_ACCESS) != 0) {
		D3D11_UNORDERED_ACCESS_VIEW_DESC d3dUAVDesc = {};
		d3dUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
		d3dUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		d3dUAVDesc.Buffer.FirstElement = 0;
		d3dUAVDesc.Buffer.NumElements = desc.byteWidth / desc.stride;

		DX("Failed to create D3D11 unordered access view",
		   device->CreateUnorderedAccessView(buffer, &d3dUAVDesc, &uav));
	}

	return true;
}

void CD3D11ManagedBuffer::Destroy() {
	if (buffer != nullptr) {
		buffer->Release();
		buffer = nullptr;
	}

	if (srv != nullptr) {
		srv->Release();
		srv = nullptr;
	}

	if (uav != nullptr) {
		uav->Release();
		uav = nullptr;
	}
}

void CD3D11ManagedBuffer::AttachToContext(IRenderContext *context) {
	if (context->GetRenderAPI() != ContextRenderAPI::D3D11) {
		throw std::runtime_error(
			"CD3D11ManagedBuffer can only be attached to a D3D11 context"
		);
	}

	this->context = context;
}

void *CD3D11ManagedBuffer::GetBufferResource() { return buffer; }

GellyObserverPtr<ID3D11ShaderResourceView> CD3D11ManagedBuffer::GetSRV() {
	return srv;
}

GellyObserverPtr<ID3D11UnorderedAccessView> CD3D11ManagedBuffer::GetUAV() {
	return uav;
}