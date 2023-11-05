#include "fluidsim/CD3D11CPUSimData.h"

#include <cassert>

CD3D11CPUSimData::CD3D11CPUSimData(ISimContext *context)
	: device(reinterpret_cast<ID3D11Device *>(
		  context->GetRenderAPIHandle(RenderAPIHandle::D3D11Device)
	  )),
	  deviceContext(reinterpret_cast<ID3D11DeviceContext *>(
		  context->GetRenderAPIHandle(RenderAPIHandle::D3D11DeviceContext)
	  )) {
#ifdef _DEBUG
	assert(device != nullptr);
	assert(deviceContext != nullptr);
#endif
}

void CD3D11CPUSimData::Initialize(int maxParticles) {
	positions.Init(
		device,
		maxParticles,
		nullptr,
		static_cast<D3D11_BIND_FLAG>(
			D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS
		),
		D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ
	);
}

void *CD3D11CPUSimData::GetRenderBuffer(SimBuffer buffer) {
	switch (buffer) {
		case SimBuffer::Position:
			return positions.Get();
		default:
			return nullptr;
	}
}

SimFloat4 *CD3D11CPUSimData::MapBuffer(SimBuffer buffer) {
	ID3D11Buffer *targetBuffer = nullptr;
	switch (buffer) {
		case SimBuffer::Position:
			targetBuffer = positions.Get();
			break;
		default:
			return nullptr;
	}

	if (!targetBuffer) {
		return nullptr;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	DX("Failed to map buffer",
	   deviceContext->Map(
		   targetBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource
	   ));

	return reinterpret_cast<SimFloat4 *>(mappedResource.pData);
}

void CD3D11CPUSimData::UnmapBuffer(SimBuffer buffer) {
	ID3D11Buffer *targetBuffer = nullptr;
	switch (buffer) {
		case SimBuffer::Position:
			targetBuffer = positions.Get();
			break;
		default:
			return;
	}

	if (!targetBuffer) {
		return;
	}

	deviceContext->Unmap(targetBuffer, 0);
}

SimDataAPI CD3D11CPUSimData::GetAPI() { return SimDataAPI::D3D11; }