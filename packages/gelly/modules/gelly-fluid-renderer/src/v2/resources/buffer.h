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
 * @tparam T The element type for the buffer. This abstracts any stride
 * calculations
 */
template <typename T>
class Buffer<T> {
public:
	constexpr static unsigned int STRIDE = sizeof(T);

	struct BufferCreateInfo {
		const std::shared_ptr<Device> device;
		const unsigned int maxElementCount;

		T *initialData;
		D3D11_USAGE usage;
		DXGI_FORMAT format;
		UINT cpuAccessFlags = 0;
		UINT miscFlags = 0;
		UINT bindFlags;
	};

	Buffer(const BufferCreateInfo &createInfo);
	~Buffer() = default;

	auto GetRawBuffer() -> ComPtr<ID3D11Buffer>;
	auto GetShaderResourceView() -> ComPtr<ID3D11ShaderResourceView>;
	auto GetUnorderedAccessView() -> ComPtr<ID3D11UnorderedAccessView>;

private:
	BufferCreateInfo createInfo;

	ComPtr<ID3D11Buffer> buffer;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	ComPtr<ID3D11UnorderedAccessView> unorderedAccessView;

	auto CreateBuffer() -> ComPtr<ID3D11Buffer>;
	auto CreateShaderResourceView(const ComPtr<ID3D11Buffer> &buffer)
		-> ComPtr<ID3D11ShaderResourceView>;
	auto CreateUnorderedAccessView(const ComPtr<ID3D11Buffer> &buffer)
		-> ComPtr<ID3D11UnorderedAccessView>;
};

}  // namespace renderer
}  // namespace gelly

#endif	// BUFFER_H
