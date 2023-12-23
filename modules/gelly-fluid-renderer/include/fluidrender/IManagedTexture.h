#ifndef GELLY_IMANAGEDTEXTURE_H
#define GELLY_IMANAGEDTEXTURE_H

#include <GellyInterface.h>
#include <GellyObserverPtr.h>
#include <d3d11.h>
#include <dxgiformat.h>

#include <cstdint>
#include <optional>
#include <stdexcept>

#include "IManagedDepthBuffer.h"

class IRenderContext;

namespace Gelly {
enum class TextureBindStage : uint8_t {
	PIXEL_SHADER_READ,
	RENDER_TARGET_OUTPUT,
};

enum class TextureFormat : uint8_t {
	R8G8B8A8_UNORM,
	R32G32B32A32_FLOAT,
	R16G16B16A16_FLOAT,	 // mainly for shared textures
	R32G32_FLOAT,
};

/**
 * Texture access flags.
 * These are used to determine how the texture will be used.
 * For example, using D3D11, a texture that is read from and written to will
 * be created with an accompanying SRV and UAV.
 * A texture that is only read from will only have an SRV.
 */
enum class TextureAccess : uint8_t {
	READ = 0b01,
	WRITE = 0b10,
};

enum class TextureFilter : uint8_t {
	POINT,
	LINEAR,
};

enum class TextureAddressMode : uint8_t {
	WRAP,
};

struct TextureDesc {
	TextureFormat format{};
	TextureAccess access{};
	TextureFilter filter = TextureFilter::POINT;
	TextureAddressMode addressMode = TextureAddressMode::WRAP;

	uint16_t width{};
	uint16_t height{};

	bool isFullscreen = false;
};

/**
 * Resources associated with a texture that may be accessed by the users
 * of a texture.
 */
enum class TextureResource : uint8_t {
	D3D11_SRV,
	D3D11_RTV,
	D3D11_UAV,
	D3D11_RESOURCE	// The IDXGIResource
};

inline D3D11_TEXTURE_ADDRESS_MODE TextureAddressModeToD3D11(
	const TextureAddressMode &mode
) {
	switch (mode) {
		case TextureAddressMode::WRAP:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		default:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
	}
}

inline D3D11_FILTER TextureFilterToD3D11(const TextureFilter &filter) {
	switch (filter) {
		case TextureFilter::POINT:
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case TextureFilter::LINEAR:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		default:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	}
}

inline DXGI_FORMAT GetDXGIFormat(const TextureFormat format) {
	switch (format) {
		case TextureFormat::R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case TextureFormat::R32G32B32A32_FLOAT:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case TextureFormat::R16G16B16A16_FLOAT:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case TextureFormat::R32G32_FLOAT:
			return DXGI_FORMAT_R32G32_FLOAT;
		default:
			throw std::logic_error("Invalid texture format");
	}
}
}  // namespace Gelly

using namespace Gelly;

constexpr enum TextureAccess operator&(
	const enum TextureAccess a, const enum TextureAccess b
) {
	return static_cast<TextureAccess>(
		static_cast<uint8_t>(a) & static_cast<uint8_t>(b)
	);
}

constexpr enum TextureAccess operator|(
	const enum TextureAccess a, const enum TextureAccess b
) {
	return static_cast<TextureAccess>(
		static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
	);
}

constexpr bool operator==(const enum TextureAccess a, const int b) {
	return static_cast<uint8_t>(a) == static_cast<uint8_t>(b);
}

/**
 * Interface for implementing managed textures.
 * These are textures that are created and managed by the renderer, and make
 * no promises about the underlying resource's lifetime.
 *
 * However, notice that there is no standard API for accessing the
 * underlying resource. This is because many rendering APIs have different
 * ways of representing textures.
 */
gelly_interface IManagedTexture {
public:
	using OptionalDepthBuffer =
		std::optional<GellyObserverPtr<IManagedDepthBuffer>>;

	virtual ~IManagedTexture() = default;

	virtual void SetDesc(const TextureDesc &desc) = 0;
	[[nodiscard]] virtual const TextureDesc &GetDesc() const = 0;

	virtual bool Create() = 0;
	virtual void Destroy() = 0;

	virtual void AttachToContext(IRenderContext * context) = 0;
	virtual GellyObserverPtr<IRenderContext> GetParentContext() = 0;

	/**
	 * Sets the size of the texture to the size of the resolution of the
	 * attached context. Really, this is always going to be the size of the
	 * window.
	 * Automatically called if the texture is set to fullscreen.
	 */
	virtual void SetFullscreenSize() = 0;

	/**
	 * Returns the underlying rendering API's shared handle.
	 * @return
	 */
	virtual void *GetSharedHandle() = 0;
	virtual void *GetResource(TextureResource resource) = 0;

	virtual void BindToPipeline(
		TextureBindStage stage, uint8_t slot, OptionalDepthBuffer depthBuffer
	) = 0;

	virtual void Clear(const float color[4]) = 0;
	/**
	 * \brief This will usually perform a GPU copy operation, not a CPU copy.
	 * \param texture Other texture to copy to.
	 */
	virtual void CopyToTexture(GellyInterfaceRef<IManagedTexture> texture) = 0;
};

#endif	// GELLY_IMANAGEDTEXTURE_H
