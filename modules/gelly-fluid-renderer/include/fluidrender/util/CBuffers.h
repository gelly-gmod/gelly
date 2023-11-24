#ifndef CBUFFERS_H
#define CBUFFERS_H
#include "GellyInterfaceRef.h"
#include "fluidrender/IManagedBuffer.h"

namespace Gelly {
namespace util {
template <typename T>
inline void UpdateCBuffer(
	T *cpuStructure, GellyObserverPtr<IManagedBuffer> gpuBuffer
) {
	constexpr size_t size = sizeof(T);
	static_assert(
		size % 16 == 0, "CBuffer size must be a multiple of 16 bytes"
	);

	const auto updateMemory = [&](void *mappedMemory) {
		memcpy(mappedMemory, cpuStructure, size);
	};

	gpuBuffer->Modify(updateMemory);
}
}  // namespace util
}  // namespace Gelly

#endif	// CBUFFERS_H
