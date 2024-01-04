#include "detail/d3d11/SyncFlush.h"

namespace d3d11 {

void SyncFlush(ID3D11Device *device, ID3D11DeviceContext *context) {
	context->Flush();
	
	ID3D11Query *query;
	D3D11_QUERY_DESC desc = {};
	desc.Query = D3D11_QUERY_EVENT;
	device->CreateQuery(&desc, &query);

	context->End(query);
	while (context->GetData(query, nullptr, 0, 0) == S_FALSE) {
		// Yield..
	}

	query->Release();
}

}  // namespace splatting