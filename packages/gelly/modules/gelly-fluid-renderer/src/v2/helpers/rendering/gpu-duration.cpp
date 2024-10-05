#include "gpu-duration.h"

#include <stdexcept>

#include "helpers/throw-informative-exception.h"

namespace gelly {
namespace renderer {
namespace util {

GPUDuration::GPUDuration(const std::shared_ptr<Device> &device) :
	device(device),
	timestampBegin(CreateTimestampQuery()),
	timestampEnd(CreateTimestampQuery()),
	timestampDisjoint(CreateTimestampDisjointQuery()),
	timestampBeginValue(0),
	timestampEndValue(0),
	timestampDisjointData() {}

auto GPUDuration::Start() -> void {
	device->GetRawDeviceContext()->Begin(timestampDisjoint.Get());
	device->GetRawDeviceContext()->End(timestampBegin.Get());
}

auto GPUDuration::End() -> void {
	device->GetRawDeviceContext()->End(timestampEnd.Get());
	device->GetRawDeviceContext()->End(timestampDisjoint.Get());
}

auto GPUDuration::WaitForData() -> void {
	while (device->GetRawDeviceContext()->GetData(
			   timestampDisjoint.Get(),
			   &timestampDisjointData,
			   sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT),
			   0
		   ) != S_OK) {
		Sleep(0);
	}

	while (device->GetRawDeviceContext()->GetData(
			   timestampBegin.Get(), &timestampBeginValue, sizeof(UINT64), 0
		   ) != S_OK) {
		Sleep(0);
	}

	while (device->GetRawDeviceContext()->GetData(
			   timestampEnd.Get(), &timestampEndValue, sizeof(UINT64), 0
		   ) != S_OK) {
		Sleep(0);
	}
}

auto GPUDuration::GetDuration() -> float {
	WaitForData();

	auto delta = timestampEndValue - timestampBeginValue;
	auto frequency = timestampDisjointData.Frequency;

	auto deltaInSeconds =
		static_cast<float>(delta) / static_cast<float>(frequency);

	return deltaInSeconds * 1000.0f;
}

auto GPUDuration::CreateTimestampQuery() -> ComPtr<ID3D11Query> {
	ComPtr<ID3D11Query> query;
	D3D11_QUERY_DESC queryDesc = {};
	queryDesc.Query = D3D11_QUERY_TIMESTAMP;
	queryDesc.MiscFlags = 0;

	const auto result =
		device->GetRawDevice()->CreateQuery(&queryDesc, query.GetAddressOf());

	GELLY_RENDERER_THROW_ON_FAIL(
		result, std::runtime_error, "Failed to create timestamp query"
	);

	return query;
}

auto GPUDuration::CreateTimestampDisjointQuery() -> ComPtr<ID3D11Query> {
	ComPtr<ID3D11Query> query;
	D3D11_QUERY_DESC queryDesc = {};
	queryDesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	queryDesc.MiscFlags = 0;

	const auto result =
		device->GetRawDevice()->CreateQuery(&queryDesc, query.GetAddressOf());

	GELLY_RENDERER_THROW_ON_FAIL(
		result, std::runtime_error, "Failed to create timestamp disjoint query"
	);

	return query;
}

}  // namespace util
}  // namespace renderer
}  // namespace gelly