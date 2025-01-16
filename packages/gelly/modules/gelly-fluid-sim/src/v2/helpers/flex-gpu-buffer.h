#pragma once
#include <d3d11.h>

#include "NvFlex.h"

namespace gelly::simulation::helpers {
template <typename T>
class FlexGpuBuffer {
	struct CreateInfo {
		NvFlexLibrary *library;
		ID3D11Buffer *buffer;
		int maxElements;
		int stride = sizeof(T);
	};

	FlexGpuBuffer(const CreateInfo &createInfo) : info(createInfo) {
		buffer = NvFlexRegisterD3DBuffer(
			info.library, info.buffer, info.maxElements, info.stride
		);
	}

	FlexGpuBuffer() : buffer(nullptr), info({}) {};

	NvFlexBuffer *operator*() const { return buffer; }

private:
	NvFlexBuffer *buffer;
	CreateInfo info;
};
}  // namespace gelly::simulation::helpers