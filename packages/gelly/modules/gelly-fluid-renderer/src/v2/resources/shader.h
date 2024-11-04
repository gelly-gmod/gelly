#ifndef SHADER_H
#define SHADER_H

#include <d3d11.h>
#include <helpers/comptr.h>

#include <memory>
#include <type_traits>

#include "device.h"

namespace gelly {
namespace renderer {

template <typename ShaderType>
constexpr bool is_vertex_shader =
	std::is_same_v<ID3D11VertexShader, ShaderType>;
template <typename ShaderType>
constexpr bool is_pixel_shader = std::is_same_v<ID3D11PixelShader, ShaderType>;
template <typename ShaderType>
constexpr bool is_geometry_shader =
	std::is_same_v<ID3D11GeometryShader, ShaderType>;
template <typename ShaderType>
constexpr bool is_compute_shader =
	std::is_same_v<ID3D11ComputeShader, ShaderType>;
template <typename ShaderType>
constexpr bool is_valid_shader =
	is_vertex_shader<ShaderType> || is_pixel_shader<ShaderType> ||
	is_geometry_shader<ShaderType> || is_compute_shader<ShaderType>;

template <typename ShaderType>
class Shader {
	static_assert(
		is_valid_shader<ShaderType>, "ShaderType must be a D3D11 shader type"
	);

public:
	struct ShaderCreateInfo {
		const std::shared_ptr<Device> device;
		const void *shaderBlob{};
		unsigned int shaderBlobSize{};
	};

	explicit Shader(const ShaderCreateInfo &createInfo);
	~Shader() = default;

	auto GetRawShader() -> ComPtr<ShaderType>;
	auto GetBlob() -> const void *;
	auto GetBlobSize() -> unsigned int;

private:
	ShaderCreateInfo createInfo;
	ComPtr<ShaderType> shader;

	auto CreateVertexShader() -> ComPtr<ID3D11VertexShader>;
	auto CreatePixelShader() -> ComPtr<ID3D11PixelShader>;
	auto CreateGeometryShader() -> ComPtr<ID3D11GeometryShader>;
	auto CreateComputeShader() -> ComPtr<ID3D11ComputeShader>;
};

using PixelShader = Shader<ID3D11PixelShader>;
using VertexShader = Shader<ID3D11VertexShader>;
using GeometryShader = Shader<ID3D11GeometryShader>;
using ComputeShader = Shader<ID3D11ComputeShader>;

}  // namespace renderer
}  // namespace gelly

#endif	// SHADER_H
