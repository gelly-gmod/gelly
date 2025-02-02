#pragma once
#include "NvFlex.h"

namespace gelly::simulation::helpers {

template <typename T>
class FlexBuffer {
public:
	struct CreateInfo {
		NvFlexLibrary *library;
		int maxElements;
		int stride = sizeof(T);
	};

	FlexBuffer(const CreateInfo &createInfo) : info(createInfo) {
		buffer = NvFlexAllocBuffer(
			info.library, info.maxElements, info.stride, eNvFlexBufferHost
		);
	}

	FlexBuffer() : buffer(nullptr), info({}) {};

	~FlexBuffer() {
		if (buffer) {
			NvFlexFreeBuffer(buffer);
		}
	}

	T *Map() { return static_cast<T *>(NvFlexMap(buffer, eNvFlexMapWait)); }
	void Unmap() { NvFlexUnmap(buffer); }

	NvFlexBuffer *operator*() const { return buffer; }

private:
	NvFlexBuffer *buffer;
	CreateInfo info;
};

}  // namespace gelly::simulation::helpers
