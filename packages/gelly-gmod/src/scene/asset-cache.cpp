#include "asset-cache.h"

auto AssetCache::FetchAsset(const std::string &name) -> std::optional<Asset> {
	const auto it =
		std::find_if(assets.begin(), assets.end(), [&name](const Asset &asset) {
			return asset.name == name;
		});

	if (it == assets.end()) {
		return std::nullopt;
	}

	return *it;
}