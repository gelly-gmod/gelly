#include <cstddef>
#include <cstdint>
// gbp: amd_ags_x64.h

#ifndef amd_ags_x64_BINARY_H
#define amd_ags_x64_BINARY_H

namespace gbp::binaries::amd_ags_x64 {
	const uint8_t* GetData();
	const size_t GetDataSize();
	const char* GetHModuleName();
}

#endif // amd_ags_x64_BINARY_H
