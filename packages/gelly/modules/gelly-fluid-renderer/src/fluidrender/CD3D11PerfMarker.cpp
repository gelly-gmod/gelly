#include "fluidrender/CD3D11PerfMarker.h"

#include <wrl.h>

#include <string>

CD3D11PerfMarker::CD3D11PerfMarker(GellyInterfaceVal<IRenderContext> context) {
	auto *deviceContext = static_cast<ID3D11DeviceContext *>(
		context->GetRenderAPIResource(RenderAPIResource::D3D11DeviceContext)
	);

	if (FAILED(deviceContext->QueryInterface(
			IID_PPV_ARGS(perfMarker.GetAddressOf())
		))) {
		throw std::runtime_error("Failed to create perf marker");
	}

	// from then on the ComPtr will handle the release
}

void CD3D11PerfMarker::BeginEvent(const char *name) {
	auto wcharName = std::wstring(name, name + strlen(name));
	perfMarker->BeginEvent(wcharName.c_str());
}

void CD3D11PerfMarker::EndEvent() { perfMarker->EndEvent(); }
