#ifndef BUFFER_H
#define BUFFER_H

#include <d3d11.h>
#include <helpers/comptr.h>
#include <helpers/throw-informative-exception.h>

#include <memory>

#include "device.h"

namespace gelly {
namespace renderer {

/**
 * Encapsulates a D3D11 buffer resource.
 */
class Buffer {
public:
	struct BufferCreateInfo {
		const std::shared_ptr<Device> device;
		const unsigned int maxElementCount;
		const unsigned int stride = 0;

		const void *initialData;
		D3D11_USAGE usage;
		DXGI_FORMAT format;
		UINT cpuAccessFlags = 0;
		UINT miscFlags = 0;
		UINT bindFlags;

		template <typename T>
		static constexpr auto WithAutomaticStride(
			const BufferCreateInfo &createInfo
		) -> BufferCreateInfo {
			return BufferCreateInfo{
				createInfo.device,
				createInfo.maxElementCount,
				sizeof(T),
				createInfo.initialData,
				createInfo.usage,
				createInfo.format,
				createInfo.cpuAccessFlags,
				createInfo.miscFlags,
				createInfo.bindFlags
			};
		}
	};

	Buffer(const BufferCreateInfo &createInfo);
	~Buffer() = default;

	static auto CreateBuffer(const BufferCreateInfo &&createInfo)
		-> std::shared_ptr<Buffer>;

	auto GetRawBuffer() -> ComPtr<ID3D11Buffer>;
	auto GetShaderResourceView() -> ComPtr<ID3D11ShaderResourceView>;
	auto GetUnorderedAccessView() -> ComPtr<ID3D11UnorderedAccessView>;
	auto GetBufferBindFlags() -> UINT;
	auto GetBufferStride() -> UINT;
	auto GetCPUAccessFlags() -> UINT;

private:
	BufferCreateInfo createInfo;

	ComPtr<ID3D11Buffer> buffer;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	ComPtr<ID3D11UnorderedAccessView> unorderedAccessView;

	auto CreateBufferResource() -> ComPtr<ID3D11Buffer>;
	auto CreateShaderResourceView(const ComPtr<ID3D11Buffer> &buffer)
		-> ComPtr<ID3D11ShaderResourceView>;
	auto CreateUnorderedAccessView(const ComPtr<ID3D11Buffer> &buffer)
		-> ComPtr<ID3D11UnorderedAccessView>;
};

}  // namespace renderer
}  // namespace gelly

#endif	// BUFFER_H
