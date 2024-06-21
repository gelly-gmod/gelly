#ifndef TEXTURE_REGISTRY_H
#define TEXTURE_REGISTRY_H
#include <memory>

#include "resources/texture.h"

namespace gelly {
namespace renderer {
namespace splatting {

struct InternalTextures {
	std::shared_ptr<Texture> unfilteredEllipsoidDepth = nullptr;
	std::shared_ptr<Texture> unfilteredThickness = nullptr;
	std::shared_ptr<Texture> unfilteredAlbedo = nullptr;

	InternalTextures(
		const std::shared_ptr<Device> &device,
		unsigned int width,
		unsigned int height
	)
		: unfilteredEllipsoidDepth(Texture::CreateTexture(
			  {.device = device,
			   .width = width,
			   .height = height,
			   .format = DXGI_FORMAT_R32G32_FLOAT,
			   .usage = D3D11_USAGE_DEFAULT,
			   .bindFlags =
				   D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
			   .cpuAccessFlags = 0,
			   .miscFlags = 0,
			   .arraySize = 1,
			   .mipLevels = 1,
			   .name = "Unfiltered Ellipsoid Depth"}
		  )),
		  unfilteredThickness(Texture::CreateTexture(
			  {.device = device,
			   .width = width,
			   .height = height,
			   .format = DXGI_FORMAT_R16_FLOAT,
			   .usage = D3D11_USAGE_DEFAULT,
			   .bindFlags =
				   D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
			   .cpuAccessFlags = 0,
			   .miscFlags = 0,
			   .arraySize = 1,
			   .mipLevels = 1,
			   .name = "Unfiltered Thickness"}
		  )),
		  unfilteredAlbedo(Texture::CreateTexture(
			  {.device = device,
			   .width = width,
			   .height = height,
			   .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
			   .usage = D3D11_USAGE_DEFAULT,
			   .bindFlags =
				   D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
			   .cpuAccessFlags = 0,
			   .miscFlags = 0,
			   .arraySize = 1,
			   .mipLevels = 1,
			   .name = "Unfiltered Albedo"}
		  ))

			  {};
};

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly

#endif	// TEXTURE_REGISTRY_H
