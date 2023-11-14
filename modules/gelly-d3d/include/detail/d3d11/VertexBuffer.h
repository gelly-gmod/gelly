#ifndef GELLY_D3D11VERTEXBUFFER_H
#define GELLY_D3D11VERTEXBUFFER_H

#include <d3d11.h>

#include "Buffer.h"

namespace d3d11 {
template <typename Vertex>
class VertexBuffer {
private:
	Buffer<Vertex> vertices;
	D3D11_PRIMITIVE_TOPOLOGY topology;

public:
	explicit VertexBuffer(
		ID3D11Device *device,
		const Vertex *initData,
		int vertexCapacity,
		D3D11_PRIMITIVE_TOPOLOGY topology
	);

	~VertexBuffer() = default;

	void SetAtSlot(
		ID3D11DeviceContext *context, int slot, ID3D11InputLayout *layout
	) const;

	[[nodiscard]] int GetVertexCount() const { return vertices.GetCapacity(); }

	[[nodiscard]] ID3D11Buffer *GetVertexBuffer() const;
};

template <typename Vertex>
VertexBuffer<Vertex>::VertexBuffer(
	ID3D11Device *device,
	const Vertex *initData,
	int vertexCapacity,
	D3D11_PRIMITIVE_TOPOLOGY topology
)
	: vertices(
		  device,
		  vertexCapacity,
		  initData,
		  // FIXME: This is a pretty bad violation of separation of concerns.
		  // This is bound as a vertex buffer and a shader resource because
		  // of the fact that isosurface extraction requires this as a shader
		  // resource while particle rendering requires this as a vertex buffer.
		  static_cast<D3D11_BIND_FLAG>(
			  D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE
		  ),
		  D3D11_USAGE_DEFAULT,
		  0
	  ),
	  topology(topology) {}

template <typename Vertex>
void VertexBuffer<Vertex>::SetAtSlot(
	ID3D11DeviceContext *context, int slot, ID3D11InputLayout *layout
) const {
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer *buffer = vertices.Get();
	context->IASetVertexBuffers(slot, 1, &buffer, &stride, &offset);
	context->IASetPrimitiveTopology(topology);
	context->IASetInputLayout(layout);
}

template <typename Vertex>
ID3D11Buffer *VertexBuffer<Vertex>::GetVertexBuffer() const {
	return vertices.Get();
}

}  // namespace d3d11

#endif	// GELLY_D3D11VERTEXBUFFER_H
