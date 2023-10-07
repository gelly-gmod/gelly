#include <detail/d3d11/Cleanup.h>

void d3d11::CleanupRTsAndShaders(
	ID3D11DeviceContext *context, int usedViews, int usedSamplers
) {
	ID3D11ShaderResourceView
		*nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {nullptr};
	ID3D11SamplerState *nullSamplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = {
		nullptr};
	ID3D11RenderTargetView *nullRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {
		nullptr};

	context->OMSetRenderTargets(
		D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, nullRTVs, nullptr
	);

	context->PSSetShaderResources(0, usedViews, nullSRVs);
	context->PSSetSamplers(0, usedSamplers, nullSamplers);
	context->PSSetShader(nullptr, nullptr, 0);

	context->VSSetShader(nullptr, nullptr, 0);
	context->VSSetShaderResources(0, usedViews, nullSRVs);
	context->VSSetSamplers(0, usedSamplers, nullSamplers);

	context->GSSetShader(nullptr, nullptr, 0);
	context->GSSetShaderResources(0, usedViews, nullSRVs);
	context->GSSetSamplers(0, usedSamplers, nullSamplers);

	context->CSSetShader(nullptr, nullptr, 0);
	context->CSSetShaderResources(0, usedViews, nullSRVs);
	context->CSSetSamplers(0, usedSamplers, nullSamplers);

	ID3D11UnorderedAccessView *nullUAVs[D3D11_PS_CS_UAV_REGISTER_COUNT] = {
		nullptr};

	context->CSSetUnorderedAccessViews(
		0, D3D11_PS_CS_UAV_REGISTER_COUNT, nullUAVs, nullptr
	);
}
