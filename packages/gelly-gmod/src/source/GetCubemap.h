#ifndef GETCUBEMAP_H
#define GETCUBEMAP_H

#include <d3d9.h>

#include <optional>

#include "GarrysMod/Lua/SourceCompat.h"
#include "MathTypes.h"
#include "Signatures.h"

enum LightType_t {
	MATERIAL_LIGHT_DISABLE = 0,
	MATERIAL_LIGHT_POINT,
	MATERIAL_LIGHT_DIRECTIONAL,
	MATERIAL_LIGHT_SPOT,
};

enum LightType_OptimizationFlags_t {
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0 = 1,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION1 = 2,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION2 = 4,
};

struct LightDesc_t {
	LightType_t m_Type;	   //< MATERIAL_LIGHT_xxx
	Vector m_Color;		   //< color+intensity
	Vector m_Position;	   //< light source center position
	Vector m_Direction;	   //< for SPOT, direction it is pointing
	float m_Range;		   //< distance range for light.0=infinite
	float m_Falloff;	   //< angular falloff exponent for spot lights
	float m_Attenuation0;  //< constant distance falloff term
	float m_Attenuation1;  //< linear term of falloff
	float m_Attenuation2;  //< quadatic term of falloff
	float m_Theta;		   //< inner cone angle. no angular falloff
						   //< within this cone
	float m_Phi;		   //< outer cone angle

	// the values below are derived from the above settings for optimizations
	// These aren't used by DX8. . used for software lighting.
	float m_ThetaDot;
	float m_PhiDot;
	unsigned int m_Flags;

protected:
	float OneOver_ThetaDot_Minus_PhiDot;
	float m_RangeSquared;
};

using AmbientLightCube = Vector4D[6];

/**
 * \brief Since it's not localized to where ever you are rendering, this
 * function will return the currently active cubemap in the game at the camera's
 * position. \return Returns the currently active cubemap in the game.
 */
IDirect3DBaseTexture9 *GetCubemap();

/**
 * \brief Forcefully disables the "Queued Material System," as Valve calls
 * it--which is a multi-threaded D3D9 command builder mode.
 */
void DisableMaterialSystemThreading();

std::optional<LightDesc_t> GetLightDesc(int index);

int GetMaxLights();

AmbientLightCube *GetAmbientLightCube();
void SetupAmbientLightCubeHook();
void RemoveAmbientLightCubeHooks();

#endif	// GETCUBEMAP_H
