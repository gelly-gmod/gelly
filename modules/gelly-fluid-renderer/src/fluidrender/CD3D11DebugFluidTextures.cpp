#include "fluidrender/CD3D11DebugFluidTextures.h"

#include <stdexcept>

CD3D11DebugFluidTextures::CD3D11DebugFluidTextures()
	: albedo(nullptr), normal(nullptr), depth(nullptr) {}

void CD3D11DebugFluidTextures::SetFeatureTexture(
	FluidFeatureType feature, GellyObserverPtr<IManagedTexture> texture
) {
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
		default:
			throw std::logic_error(
				"CD3D11DebugFluidTextures::GetFeatureTexture() encountered an "
				"unsupported feature type"
			);
	}
}

bool CD3D11DebugFluidTextures::IsInitialized() {
	return albedo != nullptr && normal != nullptr && depth != nullptr;
}