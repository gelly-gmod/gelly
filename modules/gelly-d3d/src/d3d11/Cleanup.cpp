#include <detail/d3d11/Cleanup.h>

void d3d11::CleanupRTsAndShaders(ID3D11DeviceContext *context) {
	context->OMSetRenderTargets(0, nullptr, nullptr);
	context->PSSetShaderResources(0, 0, nullptr);
	context->PSSetSamplers(0, 0, nullptr);
	context->PSSetShader(nullptr, nullptr, 0);

	context->VSSetShader(nullptr, nullptr, 0);
	context->VSSetShaderResources(0, 0, nullptr);
	context->VSSetSamplers(0, 0, nullptr);

	context->GSSetShader(nullptr, nullptr, 0);
	context->GSSetShaderResources(0, 0, nullptr);
	context->GSSetSamplers(0, 0, nullptr);
}
