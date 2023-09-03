#ifndef GELLY_PARTICLERENDERING_H
#define GELLY_PARTICLERENDERING_H

#include "PerFrameCBuffer.h"
#include "detail/Camera.h"
#include "detail/ConstantBuffer.h"
#include "detail/DataTypes.h"
#include "rendering/Technique.h"

/**
 * This technique renders the bound points as circular point-sprites and
 * constructs the raw fluid surface. This technique is based on the paper
 * "Anisotropic screen space rendering for particle-based fluid simulation" by
 * Y. Xu et al.
 * @cite https://doi.org/10.1016/j.cag.2022.12.007
 * @note This technique requires that a vertex buffer with a 4-float format is
 * already bound to the input assembler.
 */
class ParticleRendering : public Technique {
private:
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3DBlob> vertexShaderBlob;
	ComPtr<ID3D11GeometryShader> geometryShader;
	ComPtr<ID3D11Buffer> particleBuffer;
	ComPtr<ID3D11InputLayout> particleInputLayoutBuffer;
	ConstantBuffer<PerFrameCBuffer> perFrameCBuffer;

public:
	int activeParticles{};

	ParticleRendering(ID3D11Device *device, int maxParticles);
	~ParticleRendering() override = default;

	void RunForFrame(
		ID3D11DeviceContext *context, TechniqueRTs *rts, const Camera &camera
	) override;
	[[nodiscard]] ID3D11Buffer *GetParticleBuffer() const;
};

#endif	// GELLY_PARTICLERENDERING_H
