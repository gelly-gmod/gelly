#include "fluidrender/CD3D11DebugFluidTextures.h"

#include <stdexcept>

#include "fluidrender/IRenderContext.h"

CD3D11DebugFluidTextures::CD3D11DebugFluidTextures()
	: albedo(nullptr), normal(nullptr), depth(nullptr) {}

void CD3D11DebugFluidTextures::SetFeatureTexture(
	FluidFeatureType feature, GellyObserverPtr<IManagedTexture> texture
) {
	const auto context = texture->GetParentContext();
	if (context == nullptr) {
		throw std::logic_error(
			"CD3D11DebugFluidTextures::SetFeatureTexture() encountered a "
			"texture that is not attached to a context"
		);
	}

	if (context->GetRenderAPI() != ContextRenderAPI::D3D11) {
		throw std::logic_error(
			"CD3D11DebugFluidTextures::SetFeatureTexture() encountered an "
			"unsupported render API"
		);
	}

	switch (feature) {
		case FluidFeatureType::ALBEDO:
			albedo = texture;
			break;
		case FluidFeatureType::NORMALS:
			normal = texture;
			break;
		case FluidFeatureType::DEPTH:
			depth = texture;
			break;
		case FluidFeatureType::POSITIONS:
			positions = texture;
			break;
		case FluidFeatureType::THICKNESS:
			thickness = texture;
			break;
		default:
			throw std::logic_error(
				"CD3D11DebugFluidTextures::SetFeatureTexture() encountered an "
				"unsupported feature type"
			);
	}
}

GellyObserverPtr<IManagedTexture> CD3D11DebugFluidTextures::GetFeatureTexture(
	FluidFeatureType feature
) {
	switch (feature) {
		case FluidFeatureType::ALBEDO:
			return albedo;
		case FluidFeatureType::NORMALS:
			return normal;
		case FluidFeatureType::DEPTH:
			return depth;
		case FluidFeatureType::POSITIONS:
			return positions;
		case FluidFeatureType::THICKNESS:
			return thickness;
		default:
			throw std::logic_error(
				"CD3D11DebugFluidTextures::GetFeatureTexture() encountered an "
				"unsupported feature type"
			);
	}
}

bool CD3D11DebugFluidTextures::IsInitialized() {
	return depth != nullptr && normal != nullptr && albedo != nullptr &&
		   positions != nullptr;
}