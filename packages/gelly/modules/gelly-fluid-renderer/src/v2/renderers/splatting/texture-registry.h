#ifndef TEXTURE_REGISTRY_H
#define TEXTURE_REGISTRY_H
#include <memory>

#include "resources/depth-buffer.h"
#include "resources/native-image.h"
#include "resources/shared-image.h"
#include "resources/texture.h"

namespace gelly {
namespace renderer {
namespace splatting {

struct InputSharedHandles {
	HANDLE ellipsoidDepth = nullptr;
	HANDLE thickness = nullptr;
	HANDLE albedo = nullptr;
	HANDLE normals = nullptr;
	HANDLE positions = nullptr;
};

struct OutputTextures {
	std::shared_ptr<Texture> ellipsoidDepth = nullptr;
	std::shared_ptr<Texture> thickness = nullptr;
	std::shared_ptr<Texture> albedo = nullptr;
	std::shared_ptr<Texture> normals = nullptr;
	std::shared_ptr<Texture> positions = nullptr;

	OutputTextures(
		const std::shared_ptr<Device> &device, const InputSharedHandles &handles
	) :
		ellipsoidDepth(Texture::CreateTexture(
			{.device = device,
			 .image = SharedImage::CreateSharedImage(
				 {.device = device, .sharedHandle = handles.ellipsoidDepth}
			 ),
			 .bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET}
		)),
		thickness(Texture::CreateTexture(
			{.device = device,
			 .image = SharedImage::CreateSharedImage(
				 {.device = device, .sharedHandle = handles.thickness}
			 ),
			 .bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET}
		)),
		albedo(Texture::CreateTexture(
			{.device = device,
			 .image = SharedImage::CreateSharedImage(
				 {.device = device, .sharedHandle = handles.albedo}
			 ),
			 .bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET}
		)),
		normals(Texture::CreateTexture(
			{.device = device,
			 .image = SharedImage::CreateSharedImage(
				 {.device = device, .sharedHandle = handles.normals}
			 ),
			 .bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET}
		)),
		positions(Texture::CreateTexture(
			{.device = device,
			 .image = SharedImage::CreateSharedImage(
				 {.device = device, .sharedHandle = handles.positions}
			 ),
			 .bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET}
		)) {}
};

struct InternalTextures {
	std::shared_ptr<Texture> unfilteredEllipsoidDepth = nullptr;
	std::shared_ptr<Texture> unfilteredBackEllipsoidDepth = nullptr;
	std::shared_ptr<Texture> filteredBackEllipsoidDepth = nullptr;

	std::shared_ptr<Texture> unfilteredThickness = nullptr;
	std::shared_ptr<Texture> unfilteredAlbedo = nullptr;
	std::shared_ptr<Texture> unfilteredNormals = nullptr;
	std::shared_ptr<DepthBuffer> ellipsoidDepthBuffer = nullptr;

	InternalTextures(
		const std::shared_ptr<Device> &device,
		unsigned int width,
		unsigned int height
	) :
		unfilteredEllipsoidDepth(Texture::CreateTexture(
			{.device = device,
			 .image = NativeImage::CreateNativeImage(
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
			 ),
			 .bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET}
		)),
		filteredBackEllipsoidDepth(Texture::CreateTexture(
			{.device = device,
			 .image = NativeImage::CreateNativeImage(
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
				  .name = "Filtered Back Ellipsoid Depth"}
			 ),
			 .bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET}
		)),
		unfilteredThickness(Texture::CreateTexture(
			{.device = device,
			 .image = NativeImage::CreateNativeImage(
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
			 ),
			 .bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET}
		)),
		unfilteredBackEllipsoidDepth(Texture::CreateTexture(
			{.device = device,
			 .image = NativeImage::CreateNativeImage(
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
				  .name = "Unfiltered Back Ellipsoid Depth"}
			 ),
			 .bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET}
		)),
		unfilteredAlbedo(Texture::CreateTexture(
			{.device = device,
			 .image = NativeImage::CreateNativeImage(
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
			 ),
			 .bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET}
		)),
		unfilteredNormals(Texture::CreateTexture(
			{.device = device,
			 .image = NativeImage::CreateNativeImage(
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
				  .mipLevels = 8,  // we want to have mips so that in some
								   // cases, we can increase a kernel footprint
								   // without having to use more iterations
				  .name = "Unfiltered Normals"}
			 ),
			 .bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET}
		)),
		ellipsoidDepthBuffer(DepthBuffer::CreateDepthBuffer(
			{.device = device,
			 .depthTexture = Texture::CreateTexture(
				 {.device = device,
				  .image = NativeImage::CreateNativeImage(
					  {.device = device,
					   .width = width,
					   .height = height,
					   .format = DXGI_FORMAT_D24_UNORM_S8_UINT,
					   .usage = D3D11_USAGE_DEFAULT,
					   .bindFlags = D3D11_BIND_DEPTH_STENCIL,
					   .cpuAccessFlags = 0,
					   .miscFlags = 0,
					   .arraySize = 1,
					   .mipLevels = 1,
					   .name = "Ellipsoid depth buffer"}
				  )}
			 )}
		)){};
};

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly

#endif	// TEXTURE_REGISTRY_H
