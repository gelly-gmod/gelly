#ifndef ASSET_CACHE_H
#define ASSET_CACHE_H
#include <optional>
#include <string>
#include <vector>

class AssetCache {
public:
	struct Bone {
		std::string name;
		std::vector<float> vertices;
	};

	struct Asset {
		/**
		 * Typically the path to the model file, or a star followed by a number
		 * for brush models.
		 */
		std::string name;
		std::vector<Bone> bones;
	};

	AssetCache() = default;
	~AssetCache() = default;

	/**
	 * Fetches an asset from the cache.
	 * @param name Name of the asset
	 * @return An optional containing the asset if it exists, or an empty
	 * optional if it does not.
	 */
	auto FetchAsset(const std::string &name) -> std::optional<Asset>;

	/**
	 * Inserts an asset into the cache, overwriting any existing asset with the
	 * same name.
	 */
	auto InsertAsset(const std::string &name, std::vector<Bone> bones) -> void {
		const auto it = std::find_if(
			assets.begin(),
			assets.end(),
			[&name](const Asset &asset) { return asset.name == name; }
		);

		if (it != assets.end()) {
			assets.erase(it);
		}

		assets.emplace_back(name, std::move(bones));
	}

private:
	std::vector<Asset> assets;
};

#endif	// ASSET_CACHE_H
