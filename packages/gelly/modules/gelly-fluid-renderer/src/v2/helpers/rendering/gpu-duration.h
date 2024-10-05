#ifndef GPU_DURATION_H
#define GPU_DURATION_H
#include <memory>

#include "device.h"

namespace gelly {
namespace renderer {
namespace util {

class GPUDuration {
public:
	GPUDuration(const std::shared_ptr<Device> &device);
	~GPUDuration() = default;

	auto Start() -> void;
	auto End() -> void;

	/**
	 * Will forcibly end the GPU operation and return the duration, so use this
	 * at the end of a scope.
	 * @return The duration of the GPU operation in milliseconds.
	 */
	auto GetDuration() -> float;
	auto IsDisjoint() -> bool;

private:
	std::shared_ptr<Device> device;

	ComPtr<ID3D11Query> timestampBegin;
	ComPtr<ID3D11Query> timestampEnd;
	ComPtr<ID3D11Query> timestampDisjoint;

	UINT64 timestampBeginValue;
	UINT64 timestampEndValue;
	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT timestampDisjointData;

	bool dataFetched = false;

	auto CreateTimestampQuery() -> ComPtr<ID3D11Query>;
	auto CreateTimestampDisjointQuery() -> ComPtr<ID3D11Query>;

	auto WaitForData() -> void;
};

}  // namespace util
}  // namespace renderer
}  // namespace gelly
#endif	// GPU_DURATION_H
