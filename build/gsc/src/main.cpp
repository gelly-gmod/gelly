#include <windows.h>

#include <cstdio>

#include "GlueCodeGen.h"
#include "ShaderFile.h"
#include "ShaderFileCompiler.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s <shader file>\n", argv[0]);
		return 1;
	}

	const auto shaderFilePath = argv[1];
	const auto shaderFile = ShaderFile(shaderFilePath);
	const auto compiler = ShaderFileCompiler(shaderFile);
	const auto bytecode = compiler.GetBytecode();
	const auto &file = compiler.GetShaderFile();

	const auto glueCodeGen = GlueCodeGen(bytecode, file);

	printf("Writing files...\n");
	glueCodeGen.WriteFiles();
}