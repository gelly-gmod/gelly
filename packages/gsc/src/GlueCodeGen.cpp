#include "GlueCodeGen.h"

#include <fstream>

void StringReplaceAll(
	std::string &target,
	const std::string &substring,
	const std::string &replacement
) {
	auto pos = target.find(substring);
	do {
		target.replace(pos, substring.size(), replacement);
		pos = target.find(substring);
	} while (pos != std::string::npos);
}

std::string ConvertBytesToCharArray(
	const std::string &name, const std::vector<uint8_t> &bytes
) {
	std::string charArray;
	const std::string declaration = "const uint8_t " + name + "_BC[] = {";
	// 255 is the maximum number, so we'll just take 3 characters for each byte
	// "// clang-format off" is 20 characters with the newline.
	// "// clang-format on" is 19 characters with the newline.
	// "};\n" is 3 characters.

	const size_t stringSize =
		3 * bytes.size() + 20 + 19 + declaration.size() + 3;
	charArray.reserve(stringSize);

	charArray += "// clang-format off\n";
	charArray += declaration;

	for (size_t i = 0; i < bytes.size(); ++i) {
		charArray += std::to_string(bytes[i]);
		if (i != bytes.size() - 1) {
			charArray += ", ";
		}
	}

	charArray += "};\n";
	charArray += "// clang-format on\n";

	return charArray;
}

const std::string HEADER_GLUECODE_TEMPLATE = R"(
#ifndef {NAME}_GLUECODE_H
#define {NAME}_GLUECODE_H

#include <cstdint> // for uint8_t
#include <cstddef> // for size_t

namespace gsc {
namespace {NAME} {
	const uint8_t *GetBytecode();
	const size_t GetBytecodeSize();
}
}

#endif
)";

void GlueCodeGen::GenerateHeaderFile() {
	headerFile = HEADER_GLUECODE_TEMPLATE;
	const std::string name = shaderFile.GetFriendlyName();

	StringReplaceAll(headerFile, "{NAME}", name);
}

const std::string SOURCE_GLUECODE_TEMPLATE = R"(
#include "{NAME}.h"

{BYTECODE}

const uint8_t *gsc::{NAME}::GetBytecode() {
	return {NAME}_BC;
}

const size_t gsc::{NAME}::GetBytecodeSize() {
	return {BYTECODE_SIZE};
}
)";

void GlueCodeGen::GenerateSourceFile() {
	sourceFile = SOURCE_GLUECODE_TEMPLATE;
	const std::string name = shaderFile.GetFriendlyName();
	const auto &bytecode = *this->bytecode;
	const std::string bytecodeCharArray =
		ConvertBytesToCharArray(name, bytecode);

	StringReplaceAll(sourceFile, "{NAME}", name);
	StringReplaceAll(sourceFile, "{BYTECODE}", bytecodeCharArray);
	StringReplaceAll(
		sourceFile, "{BYTECODE_SIZE}", std::to_string(bytecode.size())
	);
}

GlueCodeGen::GlueCodeGen(
	ShaderFileCompiler::ShaderBytecodePtr bytecode, const ShaderFile &shaderFile
)
	: bytecode(std::move(bytecode)), shaderFile(shaderFile) {
	GenerateHeaderFile();
	GenerateSourceFile();
}

const GlueCodeGen::GlueCode &GlueCodeGen::GetHeaderFile() const {
	return headerFile;
}

const GlueCodeGen::GlueCode &GlueCodeGen::GetSourceFile() const {
	return sourceFile;
}

void GlueCodeGen::WriteFiles() const {
	const auto name = shaderFile.GetFriendlyName();
	auto headerFileName = name + ".h";
	auto sourceFileName = name + ".cpp";
	const auto &path = shaderFile.GetPath();

	auto parentPath = path.parent_path();
	auto headerPath = parentPath / "out" / headerFileName;
	auto sourcePath = parentPath / "out" / sourceFileName;
	// Create the out directory if it doesn't exist.
	if (!exists(parentPath / "out")) {
		create_directory(parentPath / "out");
	}

	std::ofstream headerFile{headerPath};
	headerFile << GetHeaderFile();
	headerFile.close();

	std::ofstream sourceFile{sourcePath};
	sourceFile << GetSourceFile();
	sourceFile.close();
}