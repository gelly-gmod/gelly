#ifndef GELLY_D3D9TEXTURE_H
#define GELLY_D3D9TEXTURE_H

#include <d3d9.h>
#include <wrl.h>

using namespace Microsoft::WRL;

namespace d3d9 {
class Texture {
private:
	ComPtr<IDirect3DTexture9> texture;
	void SetupStage(int stage, IDirect3DDevice9 *device) const;
	static void SetupSampler(int sampler, IDirect3DDevice9 *device);

public:
	HANDLE sharedHandle;
	int width;
	int height;
	D3DFORMAT format;

	Texture(
		IDirect3DTexture9 *texture,
		HANDLE sharedHandle,
		int width,
		int height,
		D3DFORMAT format
	);
	Texture(
		IDirect3DDevice9 *device,
		int width,
		int height,
		D3DFORMAT format,
		DWORD usage
	);
	~Texture() = default;

	void SetupAtStage(int stage, int sampler, IDirect3DDevice9 *device) const;
	void DuplicateSurface(IDirect3DDevice9 *device, IDirect3DSurface9 *surface);
	[[nodiscard]] IDirect3DTexture9 *Get() const;
};
}  // namespace d3d9

#endif	// GELLY_D3D9TEXTURE_H
