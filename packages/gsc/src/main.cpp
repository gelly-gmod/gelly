#include <windows.h>

#include <cstdio>

#include "GlueCodeGen.h"
#include "ShaderFile.h"
#include "ShaderFileCompiler.h"

int main(const int argc, char *argv[]) {
	if (argc < 4) {
		printf("Usage: %s <shader file> -debug <true/false>\n", argv[0]);
		return 1;
	}

	try {
		const auto shaderFilePath = argv[1];
		const auto debug =
			strcmp(argv[2], "-debug") == 0 && strcmp(argv[3], "true") == 0;

		if (debug) {
			printf("[gsc] debug mode enabled\n");
		}

		const auto shaderFile = ShaderFile(shaderFilePath, debug);
		const auto compiler = ShaderFileCompiler(shaderFile, debug);
		const auto bytecode = compiler.GetBytecode();
		const auto &file = compiler.GetShaderFile();
		const auto glueCodeGen =
			GlueCodeGen(bytecode, file, compiler.GetCompiledPath());

		glueCodeGen.WriteFiles();
		printf("[gsc] compiled %s\n", shaderFilePath);
	} catch (std::exception &exception) {
		printf("[gsc] failed to compile shader, see error output\n");
		printf("[gsc] error caught: %s\n", exception.what());
		return 1;
	}

	return 0;
}