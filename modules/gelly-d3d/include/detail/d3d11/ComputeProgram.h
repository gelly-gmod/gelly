#ifndef GELLY_COMPUTEPROGRAM_H
#define GELLY_COMPUTEPROGRAM_H

#include <d3d11.h>
#include <wrl.h>

#include "ConstantBuffer.h"

using namespace Microsoft::WRL;

namespace d3d11 {

template <typename CBuffer>
struct ComputeProgramLayout {
	int numThreadsX;
	int numThreadsY;
	ID3D11UnorderedAccessView *views[8];
	int numViews;
	ID3D11ShaderResourceView *resources[8];
	int numResources;
	ConstantBuffer<CBuffer> *constantBuffer;
};

class ComputeProgram {
private:
	ComPtr<ID3D11ComputeShader> shader;

public:
	ComputeProgram(
		ID3D11Device *device, const char *programName, const char *entryPoint
	);

	~ComputeProgram() = default;

	template <typename CBuffer>
	void Run(
		ID3D11DeviceContext *context, ComputeProgramLayout<CBuffer> &layout
	) const {
		context->CSSetShader(shader.Get(), nullptr, 0);
		context->CSSetShaderResources(0, layout.numResources, layout.resources);
		context->CSSetUnorderedAccessViews(
			0, layout.numViews, layout.views, nullptr
		);

		if (layout.constantBuffer) {
			layout.constantBuffer->BindToShaders(context, 0);
		}
		
		context->Dispatch(layout.numThreadsX, layout.numThreadsY, 1);
	}
};
}  // namespace d3d11

#endif	// GELLY_COMPUTEPROGRAM_H
