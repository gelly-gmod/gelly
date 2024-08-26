#ifndef CREATE_BINARY_PACK_HEADER_H
#define CREATE_BINARY_PACK_HEADER_H

#include <vector>

#include "pack-dll.h"

auto CreateBinaryPackHeaderFile(
	const std::string &outputDirectory, const std::vector<PackedDLL> &dlls
) -> bool;

#endif	// CREATE_BINARY_PACK_HEADER_H
