#ifndef IVRENDERVIEW_H
#define IVRENDERVIEW_H

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstddef>

#include "CViewSetup.h"
#include "MathTypes.h"

typedef void IClientEntity;
typedef void model_t;
typedef void IWorldRenderList;
typedef void VisibleFogVolumeInfo_t;
typedef void WorldListInfo_t;
typedef void VisOverrideData_t;
typedef void *IMaterial;
typedef void *colorVec;
typedef void IBrushRenderer;
typedef void IClientRenderable;
typedef void *ITexture;
typedef void *Frustum;
typedef void *ERenderDepthMode;
typedef void *DrawBrushModelMode_t;

#define VENGINE_RENDERVIEW_INTERFACE_VERSION "VEngineRenderView014"

class IVRenderView {
public:
	// Draw normal brush model.
	// If pMaterialOverride is non-null, then all the faces of the bmodel will
	// set this material rather than their regular material.
	virtual void DrawBrushModel(
		IClientEntity *baseentity,
		model_t *model,
		const Vector &origin,
		const QAngle &angles,
		bool bUnused
	) = 0;

	// Draw brush model that has no origin/angles change ( uses identity
	// transform )
	// FIXME, Material proxy IClientEntity *baseentity is unused right now, use
	// DrawBrushModel for brushes with
	//  proxies for now.
	virtual void DrawIdentityBrushModel(
		IWorldRenderList *pList, model_t *model
	) = 0;

	// Mark this dynamic light as having changed this frame ( so light maps
	// affected will be recomputed )
	virtual void TouchLight(struct dlight_t *light) = 0;
	// Draw 3D Overlays
	virtual void Draw3DDebugOverlays(void) = 0;
	// Sets global blending fraction
	virtual void SetBlend(float blend) = 0;
	virtual float GetBlend(void) = 0;

	// Sets global color modulation
	virtual void SetColorModulation(float const *blend) = 0;
	virtual void GetColorModulation(float *blend) = 0;

	// Wrap entire scene drawing
	virtual void SceneBegin(void) = 0;
	virtual void SceneEnd(void) = 0;

	// Gets the fog volume for a particular point
	virtual void GetVisibleFogVolume(
		const Vector &eyePoint, VisibleFogVolumeInfo_t *pInfo
	) = 0;

	// Wraps world drawing
	// If iForceViewLeaf is not -1, then it uses the specified leaf as your
	// starting area for setting up area portal culling. This is used by water
	// since your reflected view origin is often in solid space, but we still
	// want to treat it as though the first portal we're looking out of is a
	// water portal, so our view effectively originates under the water.
	virtual IWorldRenderList *CreateWorldList() = 0;

	virtual void BuildWorldLists(
		IWorldRenderList *pList,
		WorldListInfo_t *pInfo,
		int iForceFViewLeaf,
		const VisOverrideData_t *pVisData = nullptr,
		bool bShadowDepth = false,
		float *pReflectionWaterHeight = nullptr
	) = 0;
	virtual void DrawWorldLists(
		IWorldRenderList *pList, unsigned long flags, float waterZAdjust
	) = 0;

	// Optimization for top view
	virtual void DrawTopView(bool enable) = 0;
	virtual void TopViewBounds(Vector2D const &mins, Vector2D const &maxs) = 0;

	// Draw lights
	virtual void DrawLights(void) = 0;
	// FIXME:  This function is a stub, doesn't do anything in the engine right
	// now
	virtual void DrawMaskEntities(void) = 0;

	// Draw surfaces with alpha
	virtual void DrawTranslucentSurfaces(
		IWorldRenderList *pList,
		int sortIndex,
		unsigned long flags,
		bool bShadowDepth
	) = 0;

	// Draw Particles ( just draws the linefine for debugging map leaks )
	virtual void DrawLineFile(void) = 0;
	// Draw lightmaps
	virtual void DrawLightmaps(IWorldRenderList *pList, int pageId) = 0;
	// Wraps view render sequence, sets up a view
	virtual void ViewSetupVis(
		bool novis, int numorigins, const Vector origin[]
	) = 0;

	// Return true if any of these leaves are visible in the current PVS.
	virtual bool AreAnyLeavesVisible(int *leafList, int nLeaves) = 0;

	virtual void VguiPaint(void) = 0;
	// Sets up view fade parameters
	virtual void ViewDrawFade(std::byte *color, IMaterial *pMaterial) = 0;
	// Sets up the projection matrix for the specified field of view
	virtual void OLD_SetProjectionMatrix(
		float fov, float zNear, float zFar
	) = 0;
	// Determine lighting at specified position
	virtual colorVec GetLightAtPoint(Vector &pos) = 0;
	// Whose eyes are we looking through?
	virtual int GetViewEntity(void) = 0;
	// Get engine field of view setting
	virtual float GetFieldOfView(void) = 0;
	// 1 == ducking, 0 == not
	virtual unsigned char **GetAreaBits(void) = 0;

	// Set up fog for a particular leaf
	virtual void SetFogVolumeState(
		int nVisibleFogVolume, bool bUseHeightFog
	) = 0;

	// Installs a brush surface draw override method, null means use normal
	// renderer
	virtual void InstallBrushSurfaceRenderer(IBrushRenderer *pBrushRenderer
	) = 0;

	// Draw brush model shadow
	virtual void DrawBrushModelShadow(IClientRenderable *pRenderable) = 0;

	// Does the leaf contain translucent surfaces?
	virtual bool LeafContainsTranslucentSurfaces(
		IWorldRenderList *pList, int sortIndex, unsigned long flags
	) = 0;

	virtual bool DoesBoxIntersectWaterVolume(
		const Vector &mins, const Vector &maxs, int leafWaterDataID
	) = 0;

	virtual void SetAreaState(
		unsigned char *chAreaBits, unsigned char *chAreaPortalBits
	) = 0;

	// See i
	virtual void VGui_Paint(int mode) = 0;

	// Push, pop views (see PushViewFlags_t above for flags)
	virtual void Push3DView(
		const CViewSetup &view,
		int nFlags,
		ITexture *pRenderTarget,
		Frustum frustumPlanes
	) = 0;
	virtual void Push2DView(
		const CViewSetup &view,
		int nFlags,
		ITexture *pRenderTarget,
		Frustum frustumPlanes
	) = 0;
	virtual void PopView(Frustum frustumPlanes) = 0;

	// Sets the main view
	virtual void SetMainView(const Vector &vecOrigin, const QAngle &angles) = 0;

	enum { VIEW_SETUP_VIS_EX_RETURN_FLAGS_USES_RADIAL_VIS = 0x00000001 };

	// Wraps view render sequence, sets up a view
	virtual void ViewSetupVisEx(
		bool novis,
		int numorigins,
		const Vector origin[],
		unsigned int &returnFlags
	) = 0;

	// replaces the current view frustum with a rhyming replacement of your
	// choice
	virtual void OverrideViewFrustum(Frustum custom) = 0;

	virtual void DrawBrushModelShadowDepth(
		IClientEntity *baseentity,
		model_t *model,
		const Vector &origin,
		const QAngle &angles,
		ERenderDepthMode DepthMode
	) = 0;
	virtual void UpdateBrushModelLightmap(
		model_t *model, IClientRenderable *pRenderable
	) = 0;
	virtual void BeginUpdateLightmaps(void) = 0;
	virtual void EndUpdateLightmaps(void) = 0;
	virtual void OLD_SetOffCenterProjectionMatrix(
		float fov,
		float zNear,
		float zFar,
		float flAspectRatio,
		float flBottom,
		float flTop,
		float flLeft,
		float flRight
	) = 0;
	virtual void OLD_SetProjectionMatrixOrtho(
		float left,
		float top,
		float right,
		float bottom,
		float zNear,
		float zFar
	) = 0;
	virtual void Push3DView(
		const CViewSetup &view,
		int nFlags,
		ITexture *pRenderTarget,
		Frustum frustumPlanes,
		ITexture *pDepthTexture
	) = 0;
	virtual void GetMatricesForView(
		const CViewSetup &view,
		VMatrix *pWorldToView,
		VMatrix *pViewToProjection,
		VMatrix *pWorldToProjection,
		VMatrix *pWorldToPixels
	) = 0;
	virtual void DrawBrushModelEx(
		IClientEntity *baseentity,
		model_t *model,
		const Vector &origin,
		const QAngle &angles,
		DrawBrushModelMode_t mode
	) = 0;
};

void GetMatricesFromView(
	const CViewSetup &view,
	VMatrix *pWorldToView,
	VMatrix *pViewToProjection,
	VMatrix *pWorldToProjection,
	VMatrix *pWorldToPixels
);

#endif //IVRENDERVIEW_H
