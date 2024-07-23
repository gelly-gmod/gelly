#ifndef PACK_DLL_H
#define PACK_DLL_H

#include <optional>
#include <string>

struct PackedDLL {
	std::string name;
	std::string cppPath;
	std::string hPath;
};

auto PackDLL(const std::string &dllName, const std::string &outputDir)
	-> std::optional<PackedDLL>;

#endif	// PACK_DLL_H
