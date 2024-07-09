#ifndef PARSE_ASSET_FROM_FILESYSTEM_H
#define PARSE_ASSET_FROM_FILESYSTEM_H
#include <memory>
#include <stdexcept>

#include "GMFS.h"
#include "GarrysMod/Lua/SourceCompat.h"
#include "PHY.h"
#include "PHYToGMod.h"

namespace gelly::gmod::helpers {
inline auto ParseAssetFromFilesystem(const char *assetPath)
	-> std::vector<Vector> {
	std::string assetPathStr(assetPath);
	assetPathStr += ".phy";

	if (!FileSystem::Exists(assetPathStr.c_str())) {
		throw std::runtime_error(
			"Asset '" + assetPathStr + "' does not exist."
		);
	}

	const auto file = FileSystem::Open(assetPathStr.c_str(), "rb");
	const auto size = FileSystem::Size(file);

	auto data = std::make_unique<char[]>(size);
	FileSystem::Read(reinterpret_cast<uint8_t *>(data.get()), size, file);
	FileSystem::Close(file);

	const auto phy = PHYParser::PHY({std::move(data), size});

	std::vector<Vector> vertices;
	for (int i = 0; i < phy.GetSolidCount(); i++) {
		const auto solid = phy.GetSolid(i);
		for (const auto &triangle : solid.GetTriangles()) {
			Vector v0;
			v0.x = triangle.vertices[0].x;
			v0.y = triangle.vertices[0].y;
			v0.z = triangle.vertices[0].z;

			Vector v1;
			v1.x = triangle.vertices[1].x;
			v1.y = triangle.vertices[1].y;
			v1.z = triangle.vertices[1].z;

			Vector v2;
			v2.x = triangle.vertices[2].x;
			v2.y = triangle.vertices[2].y;
			v2.z = triangle.vertices[2].z;

			ConvertPHYPositionToGMod(v0.x, v0.y, v0.z);
			ConvertPHYPositionToGMod(v1.x, v1.y, v1.z);
			ConvertPHYPositionToGMod(v2.x, v2.y, v2.z);

			vertices.push_back(v0);
			vertices.push_back(v1);
			vertices.push_back(v2);
		}
	}

	return std::move(vertices);
}
}  // namespace gelly::gmod::helpers

#endif	// PARSE_ASSET_FROM_FILESYSTEM_H
