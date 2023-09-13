#include <d3d11.h>
#include <wrl.h>

#ifndef GELLY_D3D11TEXTURE_H
#define GELLY_D3D11TEXTURE_H

using namespace Microsoft::WRL;

namespace d3d9 {
class Texture;
}

namespace d3d11 {
class Texture {
private:
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11RenderTargetView> rtv;
	ComPtr<ID3D11ShaderResourceView> srv;
	ComPtr<ID3D11SamplerState> sampler;

public:
	Texture(const d3d9::Texture &d3d9Texture, ID3D11Device *device);
	// Default constructor for when we don't have the D3D9 texture yet.
	Texture() = default;
	~Texture() = default;

	void SetAsRT(ID3D11DeviceContext *context, ID3D11DepthStencilView *dsv)
		const;
	void SetAsSR(ID3D11DeviceContext *context, int slot) const;
	void SetSampler(ID3D11DeviceContext *context, int slot) const;

	void Clear(ID3D11DeviceContext *context, const float color[4]) const;

	[[nodiscard]] ID3D11RenderTargetView *GetRTV() const;
};
}  // namespace d3d11

#endif	// GELLY_D3D11TEXTURE_H
