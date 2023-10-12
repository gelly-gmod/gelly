#ifndef GELLY_SOLVERCONTEXT_H
#define GELLY_SOLVERCONTEXT_H

#include <GellyD3D.h>
#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class SolverContext {
private:
	ID3D11Device *device;
	ID3D11DeviceContext *deviceContext;

public:
	/**
	 * This class enables access to the passed D3D11 device and device context
	 * to the solver. Pass a raw pointer to each to make sure that nothing
	 * happens with reference counting.
	 * @param deviceReference
	 * @param deviceContextReference
	 */
	SolverContext(
		ID3D11Device *deviceReference,
		ID3D11DeviceContext *deviceContextReference
	);
	~SolverContext() = default;

	[[nodiscard]] ID3D11Device *GetDevice() const;
	[[nodiscard]] ID3D11DeviceContext *GetDeviceContext() const;

	template <typename T>
	d3d11::Buffer<T> CreateBuffer(
		size_t capacity,
		D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
		UINT cpuAccessFlags = 0,
		UINT bindFlags = D3D11_BIND_SHADER_RESOURCE
	) {
		return d3d11::Buffer<T>(
			device,
			capacity,
			nullptr,
			static_cast<D3D11_BIND_FLAG>(bindFlags),
			usage,
			cpuAccessFlags
		);
	}
};

#endif	// GELLY_SOLVERCONTEXT_H
