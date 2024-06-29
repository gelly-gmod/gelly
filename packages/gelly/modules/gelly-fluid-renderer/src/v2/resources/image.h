#ifndef IMAGE_H
#define IMAGE_H

#include <d3d11.h>
#include <helpers/comptr.h>

namespace gelly {
namespace renderer {

class Image {
public:
	Image() = default;
	virtual ~Image() = default;

	virtual auto GetTexture2D() -> ComPtr<ID3D11Texture2D> = 0;

	[[nodiscard]] virtual [[nodiscard]] auto GetWidth() const
		-> unsigned int = 0;
	[[nodiscard]] virtual auto GetHeight() const -> unsigned int = 0;
	[[nodiscard]] virtual auto GetFormat() const -> DXGI_FORMAT = 0;
	[[nodiscard]] virtual auto GetMipLevels() const -> UINT = 0;
};

}  // namespace renderer
}  // namespace gelly

#endif	// IMAGE_H
