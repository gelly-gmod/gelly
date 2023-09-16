#ifndef GELLY_SSTECHNIQUE_H
#define GELLY_SSTECHNIQUE_H

#include <GellyD3D.h>

#include "detail/Camera.h"
#include "rendering/Technique.h"

/**
 * Screen-space technique. This is a base class which sets up a screen-space
 * quad for rendering.
 * @note This technique only requires that the sub-class sets up a pixel shader.
 * The pixel shader will receive the position and texcoord of the quad.
 */
class SSTechnique : public Technique {
private:
	struct Vertex {
		float x, y, z, w;
		float u, v;
	};

	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3DBlob> vertexShaderBlob;
	ComPtr<ID3D11InputLayout> inputLayout;
	d3d11::VertexBuffer<Vertex> vertexBuffer;

protected:
	void BindNDCQuad(ID3D11DeviceContext *context);

public:
	explicit SSTechnique(ID3D11Device *device);
	~SSTechnique() override = default;
};

#endif	// GELLY_SSTECHNIQUE_H
