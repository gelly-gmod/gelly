#include "fluidrender/CD3D11ManagedDepthBuffer.h"

#include "fluidrender/IRenderContext.h"

static DXGI_FORMAT DepthFormatToDXGI(Gelly::DepthFormat format) {
	switch (format) {
		case Gelly::DepthFormat::D24S8:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		default:
			throw std::logic_error(
				"DepthFormatToDXGI: invalid format specified."
			);
	}
}

static D3D11_COMPARISON_FUNC DepthOpToD3D11(const Gelly::DepthOp depthOp) {
	switch (depthOp) {
		case Gelly::DepthOp::LESS:
			return D3D11_COMPARISON_LESS;
		case Gelly::DepthOp::LESS_EQUAL:
			return D3D11_COMPARISON_LESS_EQUAL;
		case Gelly::DepthOp::GREATER:
			return D3D11_COMPARISON_GREATER;
		case Gelly::DepthOp::GREATER_EQUAL:
			return D3D11_COMPARISON_GREATER_EQUAL;
		default:
			throw std::logic_error("DepthOpToD3D11: invalid depth op specified."
			);
	}
}

CD3D11ManagedDepthBuffer::CD3D11ManagedDepthBuffer()
	: depthStencilState(nullptr),
	  depthStencilView(nullptr),
	  depthStencilTexture(nullptr),
	  context(nullptr) {}

CD3D11ManagedDepthBuffer::~CD3D11ManagedDepthBuffer() {
	CD3D11ManagedDepthBuffer::Destroy();
}

void CD3D11ManagedDepthBuffer::SetDesc(const Gelly::DepthBufferDesc &desc) {
	this->desc = desc;
}

Gelly::DepthBufferDesc CD3D11ManagedDepthBuffer::GetDesc() const {
	return desc;
}

void CD3D11ManagedDepthBuffer::AttachToContext(
	GellyInterfaceVal<IRenderContext> context
) {
	if (context->GetRenderAPI() != ContextRenderAPI::D3D11) {
		throw std::logic_error(
			"CD3D11ManagedDepthBuffer::AttachToContext: context must be backed "
			"by D3D11"
		);
	}

	this->context = context;
}

void CD3D11ManagedDepthBuffer::Clear(float depth) {
	auto *deviceContext = static_cast<ID3D11DeviceContext *>(
		context->GetRenderAPIResource(RenderAPIResource::D3D11DeviceContext)
	);

	deviceContext->ClearDepthStencilView(
		depthStencilView, D3D11_CLEAR_DEPTH, depth, 0
	);
}

void CD3D11ManagedDepthBuffer::BindState() {
	auto *deviceContext = static_cast<ID3D11DeviceContext *>(
		context->GetRenderAPIResource(RenderAPIResource::D3D11DeviceContext)
	);

	deviceContext->OMSetDepthStencilState(depthStencilState, 1);
}

void CD3D11ManagedDepthBuffer::Create() {
	auto *device = static_cast<ID3D11Device *>(
		context->GetRenderAPIResource(RenderAPIResource::D3D11Device)
	);

	auto format = DepthFormatToDXGI(desc.format);
	auto depthOp = DepthOpToD3D11(desc.depthOp);

	uint16_t width = 0, height = 0;
	context->GetDimensions(width, height);

	D3D11_TEXTURE2D_DESC depthStencilTextureDesc{};
	depthStencilTextureDesc.Width = width;
	depthStencilTextureDesc.Height = height;
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.ArraySize = 1;
	depthStencilTextureDesc.Format = format;
	depthStencilTextureDesc.SampleDesc.Count = 1;
	depthStencilTextureDesc.SampleDesc.Quality = 0;
	depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilTextureDesc.CPUAccessFlags = 0;
	depthStencilTextureDesc.MiscFlags = 0;

	if (const auto hr = device->CreateTexture2D(
			&depthStencilTextureDesc, nullptr, &depthStencilTexture
		);
		FAILED(hr)) {
		throw std::runtime_error(
			"CD3D11ManagedDepthBuffer::Create: failed to create depth stencil "
			"texture."
		);
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = 0;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (const auto hr = device->CreateDepthStencilView(
			depthStencilTexture, &depthStencilViewDesc, &depthStencilView
		);
		FAILED(hr)) {
		throw std::runtime_error(
			"CD3D11ManagedDepthBuffer::Create: failed to create depth stencil "
			"view."
		);
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = depthOp;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	if (const auto hr = device->CreateDepthStencilState(
			&depthStencilDesc, &depthStencilState
		);
		FAILED(hr)) {
		throw std::runtime_error(
			"CD3D11ManagedDepthBuffer::Create: failed to create depth stencil "
			"state."
		);
	}
}

void CD3D11ManagedDepthBuffer::Destroy() {
	if (depthStencilState != nullptr) {
		depthStencilState->Release();
		depthStencilState = nullptr;
	}

	if (depthStencilView != nullptr) {
		depthStencilView->Release();
		depthStencilView = nullptr;
	}

	if (depthStencilTexture != nullptr) {
		depthStencilTexture->Release();
		depthStencilTexture = nullptr;
	}
}

void *CD3D11ManagedDepthBuffer::RequestResource(
	const Gelly::DepthBufferResource resource
) {
	switch (resource) {
		case Gelly::DepthBufferResource::D3D11_DSV:
			return depthStencilView;
		default:
			return nullptr;
	}
}