#ifndef DEPTH_BUFFER_H
#define DEPTH_BUFFER_H
#include <memory>

#include "device.h"
#include "texture.h"

namespace gelly {
namespace renderer {

/**
 * Enables D3D11 to access a texture as the depth buffer. Depth state is
 * generally command-invariant, so the RenderPass takes care of changign those
 * settings.
 */
class DepthBuffer {
public:
	struct DepthBufferCreateInfo {
		const std::shared_ptr<Device> device;
		const std::shared_ptr<Texture> depthTexture;
	};

	DepthBuffer(const DepthBufferCreateInfo &createInfo);
	~DepthBuffer() = default;

	auto GetDepthStencilView() -> ComPtr<ID3D11DepthStencilView>;

private:
	DepthBufferCreateInfo createInfo;
	ComPtr<ID3D11DepthStencilView> depthStencilView;

	auto CreateDepthStencilView() -> ComPtr<ID3D11DepthStencilView>;
};

}  // namespace renderer
}  // namespace gelly

#endif	// DEPTH_BUFFER_H
