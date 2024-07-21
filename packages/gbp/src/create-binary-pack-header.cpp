#include "create-binary-pack-header.h"

#include <filesystem>
#include <fstream>

#include "replace-all-occurrences.h"

const std::string BINARY_PACK_HEADER_TEMPLATE =
	R"(// gbp binary pack header: {TOTAL_DLLS} modules in this pack.
#ifndef GBP_BINARY_PACK_H
#define GBP_BINARY_PACK_H

#include <cstddef>
#include <cstdint>
#include <cstring>

// auto-generated includes
{MODULE_INCLUDES}

namespace gbp {
	struct PackedBinary {
		const char* moduleName;
		const uint8_t* data;
		const size_t dataSize;
	};

	const PackedBinary packedBinaries[] = {
{MODULE_DATA}
	};

	constexpr size_t packedBinariesSize = {TOTAL_DLLS};

	inline auto GetBinary(const char* moduleName) -> const PackedBinary* {
		for (size_t i = 0; i < packedBinariesSize; i++) {
			if (strcmp(moduleName, packedBinaries[i].moduleName) == 0) {
				return &packedBinaries[i];
			}
		}

		return nullptr;
	}
}

#endif
)";

const std::string BINARY_PACK_MODULE_DATA_TEMPLATE = R"({
	binaries::{NAME}::GetHModuleName(),
	binaries::{NAME}::GetData(),
	binaries::{NAME}::GetDataSize()
})";

auto GenerateIncludesForModules(const std::vector<PackedDLL> &dlls)
	-> std::string {
	std::string includes;
	for (const auto &dll : dlls) {
		includes += "#include \"gbp/" + dll.name + ".h\"\n";
	}

	return includes;
}

auto GeneratePackedBinaryDeclarations(const std::vector<PackedDLL> &dlls)
	-> std::string {
	std::string declarations;
	for (size_t i = 0; i < dlls.size(); i++) {
		const auto &dll = dlls[i];
		auto declaration = BINARY_PACK_MODULE_DATA_TEMPLATE;
		ReplaceAllOccurrences(declaration, "{NAME}", dll.name);

		if (i != dlls.size() - 1) {
			declaration += ",\n";
		}

		declarations += declaration;
	}

	return declarations;
}

auto CreateBinaryPackHeaderFile(
	const std::string &outputDirectory, const std::vector<PackedDLL> &dlls
) -> bool {
	std::filesystem::path outputPath = outputDirectory;
	outputPath /= "gbp.h";

	const auto moduleIncludes = GenerateIncludesForModules(dlls);
	const auto moduleData = GeneratePackedBinaryDeclarations(dlls);
	const auto stringifiedTotalDLLs = std::to_string(dlls.size());

	auto header = BINARY_PACK_HEADER_TEMPLATE;
	ReplaceAllOccurrences(header, "{MODULE_INCLUDES}", moduleIncludes);
	ReplaceAllOccurrences(header, "{MODULE_DATA}", moduleData);
	ReplaceAllOccurrences(header, "{TOTAL_DLLS}", stringifiedTotalDLLs);

	std::ofstream file(outputPath);
	if (!file.is_open()) {
		return false;
	}

	file << header;
	file.close();

	return true;
}
