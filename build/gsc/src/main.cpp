#include <windows.h>

#include <cstdio>

#include "ShaderFile.h"
#include "ShaderFileCompiler.h"
#include "ShellCmd.h"
int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s <shader file>\n", argv[0]);
		return 1;
	}

	const auto shaderFilePath = argv[1];
	auto shaderFile = ShaderFile(shaderFilePath);
	auto compiler = ShaderFileCompiler(shaderFile);

}