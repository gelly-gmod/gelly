#include "fluidrender/CD3D11ManagedBufferLayout.h"

#include <stdexcept>
#include <string>
#include <vector>

#include "fluidrender/IRenderContext.h"

DXGI_FORMAT BufferLayoutFormatToDXGI(const BufferLayoutFormat &format) {
	switch (format) {
		case BufferLayoutFormat::FLOAT2:
			return DXGI_FORMAT_R32G32_FLOAT;
		case BufferLayoutFormat::FLOAT4:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		default:
			return DXGI_FORMAT_UNKNOWN;
	}
}

CD3D11ManagedBufferLayout::CD3D11ManagedBufferLayout()
	: layout(nullptr), desc{}, context(nullptr){};

CD3D11ManagedBufferLayout::~CD3D11ManagedBufferLayout() {
	CD3D11ManagedBufferLayout::Destroy();
}

const Gelly::BufferLayoutDesc &CD3D11ManagedBufferLayout::GetLayoutDesc() {
	return desc;
}

void CD3D11ManagedBufferLayout::SetLayoutDesc(
	const Gelly::BufferLayoutDesc &desc
) {
	this->desc = desc;
}

void CD3D11ManagedBufferLayout::AttachToContext(
	GellyObserverPtr<IRenderContext> context
) {
	this->context = context;
}

void CD3D11ManagedBufferLayout::Create() {
	auto *device = static_cast<ID3D11Device *>(
		context->GetRenderAPIResource(RenderAPIResource::D3D11Device)
	);

	const auto &items = desc.items;
	const auto itemCount = desc.itemCount;

	std::vector<D3D11_INPUT_ELEMENT_DESC> d3dElements(itemCount);
	for (size_t i = 0; i < itemCount; i++) {
		const auto &element = items[i];

		d3dElements[i] = D3D11_INPUT_ELEMENT_DESC{
			element.semanticName,
			element.semanticIndex,
			BufferLayoutFormatToDXGI(element.format),
			element.slotIndex,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
		};
	}

	ID3D11InputLayout *inputLayout;
	if (const auto result = device->CreateInputLayout(
			d3dElements.data(),
			itemCount,
			desc.vertexShader->GetBytecode(),
			desc.vertexShader->GetBytecodeSize(),
			&inputLayout
		);
		FAILED(result)) {
		throw std::runtime_error(
			"Failed to create input layout: " + std::to_string(result)
		);
	}

	layout = inputLayout;
}

void CD3D11ManagedBufferLayout::Destroy() {
	if (layout != nullptr) {
		layout->Release();
		layout = nullptr;
	}
}

void CD3D11ManagedBufferLayout::AttachBufferAtSlot(
	GellyObserverPtr<IManagedBuffer> buffer, uint8_t slot
) {
	if (slot >= 8) {
		throw std::runtime_error("Slot index out of range");
	}

	if (buffer->GetDesc().type != BufferType::VERTEX) {
		throw std::runtime_error(
			"Cannot attach buffer to vertex buffer layout if it's not a vertex "
			"buffer"
		);
	}

	buffers[slot] = buffer;
}

void CD3D11ManagedBufferLayout::BindAsVertexBuffer() {
	if (layout == nullptr) {
		throw std::runtime_error(
			"Cannot bind buffer layout as vertex buffer before creating it"
		);
	}

	auto *deviceContext = static_cast<ID3D11DeviceContext *>(
		context->GetRenderAPIResource(RenderAPIResource::D3D11DeviceContext)
	);

	deviceContext->IASetInputLayout(layout);
	for (int slot = 0; slot < 8; slot++) {
		if (buffers[slot] != nullptr) {
			buffers[slot]->BindToPipeline(ShaderType::Vertex, slot);
		}
	}
}
