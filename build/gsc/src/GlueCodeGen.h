#ifndef GLUECODEGEN_H
#define GLUECODEGEN_H
#include <string>

#include "ShaderFileCompiler.h"

class GlueCodeGen {
	using GlueCode = std::string;

private:
	GlueCode headerFile;
	GlueCode sourceFile;

	ShaderFileCompiler::ShaderBytecodePtr bytecode;
	const ShaderFile &shaderFile;

	void GenerateHeaderFile();
	void GenerateSourceFile();

public:
	explicit GlueCodeGen(
		ShaderFileCompiler::ShaderBytecodePtr bytecode,
		const ShaderFile &shaderFile
	);

	[[nodiscard]] const GlueCode &GetHeaderFile() const;
	[[nodiscard]] const GlueCode &GetSourceFile() const;
	void WriteFiles() const;
};

#endif	// GLUECODEGEN_H
