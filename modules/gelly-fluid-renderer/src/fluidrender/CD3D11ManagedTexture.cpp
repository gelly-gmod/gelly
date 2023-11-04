#include "fluidrender/CD3D11ManagedTexture.h"

#include <GellyD3D.h>

#include <stdexcept>

CD3D11ManagedTexture::CD3D11ManagedTexture()
	: context(nullptr),
	  texture(nullptr),
	  srv(nullptr),
	  rtv(nullptr),
	  desc({}) {}

CD3D11ManagedTexture::~CD3D11ManagedTexture() {
	// Unfortunate, but necessary
	// There is UB if a virtual function is called in a destructor
	CD3D11ManagedTexture::Destroy();
}

void CD3D11ManagedTexture::SetDesc(const TextureDesc &desc) {
	this->desc = desc;
}

const TextureDesc &CD3D11ManagedTexture::GetDesc() const { return desc; }

bool CD3D11ManagedTexture::Create() {
	if (context == nullptr) {
		return false;
	}

	if (texture != nullptr) {
		return false;
	}

	if (desc.isFullscreen) {
		SetFullscreenSize();
	}

	auto *device = static_cast<ID3D11Device *>(
		context->GetRenderAPIResource(RenderAPIResource::D3D11Device)
	);

	auto format = static_cast<DXGI_FORMAT>(0);

	switch (desc.format) {
		case TextureFormat::R8G8B8A8_UNORM:
			format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case TextureFormat::R32G32B32A32_FLOAT:
			format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		case TextureFormat::R16G16B16A16_FLOAT:
			format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			break;
	}

	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = desc.width;
	texDesc.Height = desc.height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;

	if ((desc.access & TextureAccess::READ) != 0) {
		texDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}

	if ((desc.access & TextureAccess::WRITE) != 0) {
		texDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}

	DX("Failed to create D3D11 texture",
	   device->CreateTexture2D(&texDesc, nullptr, &texture));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	DX("Failed to create D3D11 RTV",
	   device->CreateRenderTargetView(texture, &rtvDesc, &rtv));

	if ((desc.access & TextureAccess::READ) != 0) {
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		DX("Failed to create D3D11 SRV",
		   device->CreateShaderResourceView(texture, &srvDesc, &srv));
	}

	if ((desc.access & TextureAccess::WRITE) != 0) {
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		DX("Failed to create D3D11 UAV",
		   device->CreateUnorderedAccessView(texture, &uavDesc, &uav));
	}

	return true;
}

void CD3D11ManagedTexture::Destroy() {
	if (texture != nullptr) {
		texture->Release();
		texture = nullptr;
	}

	if (srv != nullptr) {
		srv->Release();
		srv = nullptr;
	}

	if (rtv != nullptr) {
		rtv->Release();
		rtv = nullptr;
	}
}

void CD3D11ManagedTexture::AttachToContext(IRenderContext *context) {
	if (context->GetRenderAPI() != ContextRenderAPI::D3D11) {
		throw std::logic_error(
			"Cannot attach D3D11 texture to non-D3D11 context"
		);
	}

	this->context = context;
}

GellyObserverPtr<IRenderContext> CD3D11ManagedTexture::GetParentContext() {
	return context;
}

void CD3D11ManagedTexture::SetFullscreenSize() {
	if (!context) {
		return;
	}

	context->GetDimensions(desc.width, desc.height);
}

void *CD3D11ManagedTexture::GetSharedHandle() {
	if (!texture) {
		return nullptr;
	}

	IDXGIResource *resource;
	DX("Failed to get DXGI resource",
	   texture->QueryInterface(
		   __uuidof(IDXGIResource), reinterpret_cast<void **>(&resource)
	   ));

	HANDLE handle;
	DX("Failed to get shared handle", resource->GetSharedHandle(&handle));

	resource->Release();

	return handle;
}

void *CD3D11ManagedTexture::GetResource(TextureResource resource) {
	switch (resource) {
		case TextureResource::D3D11_SRV:
			return srv;
		case TextureResource::D3D11_RTV:
			return rtv;
		case TextureResource::D3D11_UAV:
			return uav;
		default:
			return nullptr;
	}
}