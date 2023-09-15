#include "detail/d3d11/Texture.h"

#include "detail/d3d11/ErrorHandling.h"
#include "detail/d3d9/Texture.h"

namespace d3d11 {
Texture::Texture(const d3d9::Texture &d3d9Texture, ID3D11Device *device)
	: texture(nullptr), rtv(nullptr) {
	// Open shared handle from the D3D9 texture we received
	{
		ComPtr<ID3D11Resource> resource;
		DX("Failed to open shared handle",
		   device->OpenSharedResource(
			   d3d9Texture.sharedHandle,
			   __uuidof(ID3D11Resource),
			   (void **)resource.GetAddressOf()
		   ));

		DX("Failed to get a 2D texture from the received D3D9 shared handle!",
		   resource.As(&texture));
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	DX("Failed to create render target view",
	   device->CreateRenderTargetView(texture.Get(), &rtvDesc, &rtv));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	DX("Failed to create shader resource view",
	   device->CreateShaderResourceView(texture.Get(), &srvDesc, &srv));

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	DX("Failed to create sampler state",
	   device->CreateSamplerState(&samplerDesc, &sampler));
}
}  // namespace d3d11

void d3d11::Texture::SetAsRT(
	ID3D11DeviceContext *context, ID3D11DepthStencilView *dsv
) const {
	context->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv);
}

void d3d11::Texture::SetAsSR(ID3D11DeviceContext *context, int slot) const {
	context->PSSetShaderResources(slot, 1, srv.GetAddressOf());
}

void d3d11::Texture::SetSampler(ID3D11DeviceContext *context, int slot) const {
	context->PSSetSamplers(slot, 1, sampler.GetAddressOf());
}

void d3d11::Texture::Clear(ID3D11DeviceContext *context, const float color[4])
	const {
	context->ClearRenderTargetView(rtv.Get(), color);
}

ID3D11RenderTargetView *d3d11::Texture::GetRTV() const { return rtv.Get(); }

void d3d11::SetMRT(
	ID3D11DeviceContext *context,
	int numTextures,
	d3d11::Texture **textures,
	ID3D11DepthStencilView *dsv
) {
	ID3D11RenderTargetView *rtViews[numTextures];
	for (int i = 0; i < numTextures; i++) {
		rtViews[i] = textures[i]->GetRTV();
	}

	context->OMSetRenderTargets(numTextures, rtViews, dsv);
}

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