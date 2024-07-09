#ifndef ASSET_CACHE_H
#define ASSET_CACHE_H
#include <optional>
#include <string>
#include <vector>

template <typename T>
concept Vertex = std::is_standard_layout_v<std::remove_reference_t<T>> &&
				 sizeof(T) == 12 && alignof(T) == 4;

template <typename T>
concept VertexArray = requires(T array) {
	{ *array } -> Vertex;
	std::is_pointer_v<T>;
};

template <typename T>
requires Vertex<T> constexpr auto ReduceGenericVertexToFloat(T *vertex)
	-> float * {
	return reinterpret_cast<float *>(vertex);
}

class AssetCache {
public:
	struct Asset {
		/**
		 * Typically the path to the model file, or a star followed by a number
		 * for brush models.
		 */
		const std::string name;
		std::vector<float> rawVertices;
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
	 * @param name Name of the asset
	 * @param vertices A pointer to the first element of the vertex array. This
	 * should be a pointer to a vertex structure with 3 floats. Several type
	 * constraints are enforced on the vertex type to ensure that it is a valid
	 * vertex--but the point is to genericize the vertex type since we have many
	 * formats which are inserted here.
	 * @param count Number of vertices in the array
	 */
	auto InsertAsset(
		const std::string &name, const VertexArray auto &vertices, size_t count
	) -> void {
		// implicitly a template function
		auto asset = FetchAsset(name);
		if (asset.has_value()) {
			asset->rawVertices = CopyAssetData(vertices, count);
		} else {
			assets.push_back(Asset{name, CopyAssetData(vertices, count)});
		}
	}

private:
	std::vector<Asset> assets;

	auto CopyAssetData(const VertexArray auto &vertices, size_t count)
		-> std::vector<float> {
		std::vector<float> data;
		data.reserve(count * 3);
		for (size_t i = 0; i < count; ++i) {
			const auto vertex = ReduceGenericVertexToFloat(vertices + i);
			data.push_back(vertex[0]);
			data.push_back(vertex[1]);
			data.push_back(vertex[2]);
		}

		// RVO likely makes this move a no-op
		return std::move(data);
	}
};

#endif	// ASSET_CACHE_H
