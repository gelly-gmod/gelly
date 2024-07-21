#include "pack-dll.h"

#include <filesystem>
#include <fstream>
#include <vector>

#include "replace-all-occurrences.h"

const std::string HEADER_TEMPLATE = R"(#include <cstddef>
#include <cstdint>
// gbp: {NAME}.h

#ifndef {NAME}_BINARY_H
#define {NAME}_BINARY_H

namespace gbp::binaries::{NAME} {
	const uint8_t* GetData();
	const size_t GetDataSize();
	const char* GetHModuleName();
}

#endif // {NAME}_BINARY_H
)";

const std::string SOURCE_TEMPLATE = R"(#include "{NAME}.h"
// gbp: {NAME}.cpp

static uint8_t DATA[] = {
{DATA}
};

static constexpr size_t DATA_SIZE = {DATA_SIZE};
static constexpr char H_MODULE_NAME[] = "{H_MODULE_NAME}";

namespace gbp::binaries::{NAME} {
	const uint8_t* GetData() {
		return DATA;
	}

	const size_t GetDataSize() {
		return DATA_SIZE;
	}

	const char* GetHModuleName() {
		return H_MODULE_NAME;
	}
}
)";

auto ExtractDLLNameFromPath(const std::string &path) -> std::string {
	std::string name;
	for (int i = static_cast<int>(path.size() - 1); i >= 0; i--) {
		if (path[i] == '\\' || path[i] == '/') {
			break;
		}

		name.push_back(path[i]);
	}
	std::reverse(name.begin(), name.end());

	// we also want to trim out any extension
	const size_t extensionPos = name.find_first_of('.'
	);	// very important, we have double extension dlls sometimes
	if (extensionPos != std::string::npos) {
		name = name.substr(0, extensionPos);
	}

	return name;
}

auto ConvertFileToByteArray(const std::string &path) -> std::vector<uint8_t> {
	std::ifstream file(path, std::ios::binary);

	file.seekg(0, std::ios::end);
	const size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<uint8_t> bytes(fileSize);
	file.read(reinterpret_cast<char *>(bytes.data()), fileSize);
	file.close();

	return std::move(bytes);
}

auto EmitUint8ArrayFromBytes(const std::vector<uint8_t> &bytes) -> std::string {
	std::string array;
	for (size_t i = 0; i < bytes.size(); ++i) {
		// Somehow, it really is way more easier to just use std::to_string,
		// rather than doing something like formatting to %02x.
		array += std::to_string(bytes[i]);
		if (i != bytes.size() - 1) {
			array += ", ";
		}
	}

	return array;
}

auto PackDLL(const std::string &dllName, const std::string &outputDir)
	-> std::optional<PackedDLL> {
	PackedDLL packedDLL = {};
	std::filesystem::path dllPath = dllName;
	if (!exists(dllPath)) {
		printf("[gbp] DLL %s does not exist, skipping...\n", dllName.c_str());
		return std::nullopt;
	}

	std::filesystem::path headerPath = outputDir;
	std::filesystem::path sourcePath = outputDir;
	packedDLL.name = ExtractDLLNameFromPath(dllName);

	headerPath /= "gbp";
	headerPath /= packedDLL.name + ".h";
	sourcePath /= "gbp";
	sourcePath /= packedDLL.name + ".cpp";

	packedDLL.cppPath = sourcePath.string();
	packedDLL.hPath = headerPath.string();

	std::filesystem::create_directories(outputDir + "/gbp");

	const auto bytes = ConvertFileToByteArray(dllName);
	const auto data = EmitUint8ArrayFromBytes(bytes);

	auto headerFile = HEADER_TEMPLATE;
	auto sourceFile = SOURCE_TEMPLATE;

	ReplaceAllOccurrences(headerFile, "{NAME}", packedDLL.name);
	ReplaceAllOccurrences(sourceFile, "{NAME}", packedDLL.name);
	ReplaceAllOccurrences(sourceFile, "{DATA}", data);
	ReplaceAllOccurrences(
		sourceFile, "{DATA_SIZE}", std::to_string(bytes.size())
	);
	ReplaceAllOccurrences(
		sourceFile, "{H_MODULE_NAME}", dllPath.filename().string()
	);

	std::ofstream headerFileStream(headerPath);
	headerFileStream << headerFile;
	headerFileStream.close();

	std::ofstream sourceFileStream(sourcePath);
	sourceFileStream << sourceFile;
	sourceFileStream.close();

	return std::move(packedDLL);
}
