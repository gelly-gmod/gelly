#ifndef SHADERFILE_H
#define SHADERFILE_H
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class ShaderFile {
private:
	fs::path path;
	/**
	 * \brief Used in the C++ glue code generation. Basically it's a nice name
	 * that will work well with any C++ code.
	 */
	std::string friendlyName;

public:
	/**
	 * \brief May throw if the path is invalid!
	 * \param path Path to the shader file, either relative or absolute.
	 */
	ShaderFile(const fs::path &path);



	[[nodiscard]] const fs::path &GetPath() const;
	[[nodiscard]] const std::string &GetFriendlyName() const;
};

#endif	// SHADERFILE_H
