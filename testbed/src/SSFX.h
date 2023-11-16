#ifndef SSFX_H
#define SSFX_H

#include <memory>
#include <vector>

#include "ILogger.h"
#include "d3d11.h"

namespace testbed {
struct SSFXEffect {
	using TextureList = std::vector<const char *>;
	using ConstantDataPtr = std::shared_ptr<std::vector<uint8_t>>;
	const char *pixelShaderPath{};

	TextureList inputTextures;
	TextureList outputTextures;

	/**
	 * \brief Raw constant data for the specified pixel shader. This will be
	 * uploaded as a constant buffer in slot 1.
	 */
	ConstantDataPtr shaderConstantData;
};

void InitializeSSFXSystem(ILogger *newLogger, ID3D11Device *rendererDevice);
bool IsSSFXInitialized();
/**
 * \brief Registers an SSFX effect with the system.
 * \param name Name of the effect
 * \param effect The effect layout
 */
void RegisterSSFXEffect(const char *name, SSFXEffect effect);
SSFXEffect::ConstantDataPtr GetSSFXEffectConstantData(const char *name);
/**
 * \brief Ensures that the constant data on the CPU is synchronized with the
 * GPU. Use after each call to UpdateSSFXEffectConstants
 * \param name Name of the effect
 */
void UpdateSSFXEffectConstants(const char *name);
void ApplySSFXEffect(const char *name);

template <typename T>
inline void SetStructAsEffectConstant(
	T *unownedDataPtr, const char *effectName
) {
	const auto constantData = GetSSFXEffectConstantData(effectName);
	memcpy(constantData.get(), unownedDataPtr, sizeof(T));
	UpdateSSFXEffectConstants(effectName);
}
}  // namespace testbed
#endif	// SSFX_H
