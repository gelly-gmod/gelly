#include "fluidrender/CD3D11ManagedBuffer.h"

#include <GellyD3D.h>

#include <stdexcept>

#include "fluidrender/IRenderContext.h"

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
	d3dBufferDesc.StructureByteStride = desc.stride;
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

	unsigned int bindFlags = 0;
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

	auto *subresourcePtr = &d3dSubresourceData;
	if (desc.initialData == nullptr) {
		subresourcePtr = nullptr;
	}

	auto *device = static_cast<ID3D11Device *>(
		context->GetRenderAPIResource(RenderAPIResource::D3D11Device)
	);

	DX("Failed to create D3D11 buffer",
	   device->CreateBuffer(&d3dBufferDesc, subresourcePtr, &buffer));

	if ((desc.type & BufferType::SHADER_RESOURCE) != 0) {
		D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc = {};
		d3dSRVDesc.Format = GetDXGIFormat(desc.format);
		d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		d3dSRVDesc.Buffer.FirstElement = 0;
		d3dSRVDesc.Buffer.NumElements = desc.byteWidth / desc.stride;

		DX("Failed to create D3D11 shader resource view",
		   device->CreateShaderResourceView(buffer, &d3dSRVDesc, &srv));
	}

	if ((desc.type & BufferType::UNORDERED_ACCESS) != 0) {
		D3D11_UNORDERED_ACCESS_VIEW_DESC d3dUAVDesc = {};
		d3dUAVDesc.Format = GetDXGIFormat(desc.format);
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

void CD3D11ManagedBuffer::BindToPipeline(
	const ShaderType shaderType, const uint8_t slot
) {
	auto *deviceContext = static_cast<ID3D11DeviceContext *>(
		context->GetRenderAPIResource(RenderAPIResource::D3D11DeviceContext)
	);

	switch (desc.type) {
		case BufferType::CONSTANT:
			switch (shaderType) {
				case ShaderType::Vertex:
					deviceContext->VSSetConstantBuffers(slot, 1, &buffer);
					break;
				case ShaderType::Pixel:
					deviceContext->PSSetConstantBuffers(slot, 1, &buffer);
					break;
				case ShaderType::Geometry:
					deviceContext->GSSetConstantBuffers(slot, 1, &buffer);
					break;
				case ShaderType::Compute:
					deviceContext->CSSetConstantBuffers(slot, 1, &buffer);
					break;
			}
			break;
		case BufferType::SHADER_RESOURCE:
			switch (shaderType) {
				case ShaderType::Vertex:
					deviceContext->VSSetShaderResources(slot, 1, &srv);
					break;
				case ShaderType::Pixel:
					deviceContext->PSSetShaderResources(slot, 1, &srv);
					break;
				case ShaderType::Geometry:
					deviceContext->GSSetShaderResources(slot, 1, &srv);
					break;
				case ShaderType::Compute:
					deviceContext->CSSetShaderResources(slot, 1, &srv);
					break;
			}
			break;
		case BufferType::UNORDERED_ACCESS:
			switch (shaderType) {
				case ShaderType::Compute:
					deviceContext->CSSetUnorderedAccessViews(
						slot, 1, &uav, nullptr
					);
					break;
				default:
					throw std::runtime_error(
						"CD3D11ManagedBuffer::BindToPipeline attempted to "
						"setup unordered access for a non-compute shader"
					);
					break;
			}
			break;
		case BufferType::VERTEX: {
			// Shader type is not used here. But, the vertex shader is the only
			// shader that can use vertex buffers.
			const UINT stride = static_cast<UINT>(desc.stride);
			constexpr UINT offset = static_cast<UINT>(0);

			deviceContext->IASetVertexBuffers(
				slot, 1, &buffer, &stride, &offset
			);
		} break;
		default:
			throw std::runtime_error(
				"CD3D11ManagedBuffer::BindToPipeline called with an invalid "
				"buffer type"
			);
			break;
	}
}

void CD3D11ManagedBuffer::Modify(const ModifierFn &modifier) {
	if ((desc.usage & BufferUsage::DYNAMIC) == 0) {
		throw std::runtime_error(
			"CD3D11ManagedBuffer::Modify called on a non-dynamic buffer"
		);
	}

	auto *deviceContext = static_cast<ID3D11DeviceContext *>(
		context->GetRenderAPIResource(RenderAPIResource::D3D11DeviceContext)
	);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};
	DX("Failed to map D3D11 buffer",
	   deviceContext->Map(
		   buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource
	   ));

	modifier(mappedSubresource.pData);

	deviceContext->Unmap(buffer, 0);
}

GellyObserverPtr<ID3D11ShaderResourceView> CD3D11ManagedBuffer::GetSRV() const {
	return srv;
}

GellyObserverPtr<ID3D11UnorderedAccessView> CD3D11ManagedBuffer::GetUAV(
) const {
	return uav;
}