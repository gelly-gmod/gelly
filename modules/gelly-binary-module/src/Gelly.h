#ifndef GELLY_GELLY_H
#define GELLY_GELLY_H

#include <GellyEngine.h>
#include <GellyRenderer.h>
#include <d3d9.h>
#include <wrl.h>

#include <mutex>
#include <semaphore>

#include "compositor/Compositor.h"

using namespace Microsoft::WRL;

struct GellyInitParams {
	int maxParticles;
	int maxColliders;
	int width;
	int height;
	SharedTextures sharedTextures;
};

struct GellyMessage {
	enum Type {
		LoadMap,
		Update,
		Render,
		AddParticle,
		SetupCamera,
		SyncCamera,
		Clear,
		SetParticleRadius,
	};

	Type type;
	union {
		struct {
			const char *mapName;
		} loadMap;
		struct {
			float deltaTime;
		} update;
		struct {
			Vec4 position;
			Vec3 velocity;
		} addParticle;
		struct {
			float fov;
			float width;
			float height;
			float nearZ;
			float farZ;
		} setupCamera;
		struct {
			Vec3 position;
			Vec3 direction;
		} syncCamera;
		struct {
			float radius;
		} setParticleRadius;
	};
};

class RendererCompositor {
private:
	IDirect3DDevice9Ex *device;
	ComPtr<IDirect3DVertexBuffer9> screenQuad;
	// no declaration needed, we use FVF instead

	struct NDCVertex {
		// It's not in viewport space, but in NDC space.

		static const DWORD FVF = D3DFVF_XYZW | D3DFVF_TEX1;
		float x, y, z, w;
		float u, v;
	};

	ComPtr<IDirect3DVertexShader9> vertexShader;
	ComPtr<IDirect3DPixelShader9> pixelShader;

	// We're not using a ComPtr here because this is meant to be owned
	// by Gelly. There should be nothing happening with the ref count.
	SharedTextures gbuffer;

	// We have to make sure to look like we were never here to the rest of the
	// game, so we store every single previous value of any function we called.

	struct {
		_D3DTEXTUREADDRESS addressU1;
		_D3DTEXTUREADDRESS addressV1;
		_D3DTEXTUREFILTERTYPE magFilter1;
		_D3DTEXTUREFILTERTYPE minFilter1;
		_D3DTEXTUREFILTERTYPE mipFilter1;

		_D3DTEXTUREADDRESS addressU2;
		_D3DTEXTUREADDRESS addressV2;
		_D3DTEXTUREFILTERTYPE magFilter2;
		_D3DTEXTUREFILTERTYPE minFilter2;
		_D3DTEXTUREFILTERTYPE mipFilter2;

		IDirect3DVertexShader9 *vertexShader;
		IDirect3DPixelShader9 *pixelShader;
		IDirect3DVertexBuffer9 *streamSource;
		IDirect3DBaseTexture9 *texture0;
		IDirect3DBaseTexture9 *texture1;
		UINT streamOffset;
		UINT streamStride;
		DWORD fvf;
		DWORD lighting;
		DWORD ztest;
		DWORD alphaBlend;

		float constant0[4];
	} previous{};

	void CreateScreenQuad();
	void CreateShaders();
	void BindShaderResources();
	void RestorePreviousState();

public:
	struct {
		float zValue = 0.f;
	} debugConstants{};

	explicit RendererCompositor(
		IDirect3DDevice9Ex *device, SharedTextures *gbuffer
	);
	~RendererCompositor() = default;
	void Composite();
};

/**
 * Gelly is a threaded wrapper of GellyScene and GellyRenderer.
 * It integrates both of them together, but uses a separate thread for any
 * of the work due to how Garry's Mod works. (it simply doesn't work on the
 * main thread)
 */
class Gelly {
private:
	GellyScene *scene;
	GellyRenderer *renderer;

	// Semaphores are used to communicate between the main thread and the gelly
	// thread. This is because there really is no multithreading involved which
	// would require something like a mutex for resource locking at critical
	// sections, we just need to do it on a separate thread due to technical
	// limitations.

	// rx
	std::binary_semaphore mainToThread;
	// tx
	std::binary_semaphore threadToMain;

	std::thread thread;
	GellyMessage *message;

	/**
	 * Processes a message sent from the main thread.
	 * This is called by the thread, but note that the message member is usually
	 * only valid in this function.
	 */
	void ProcessMessage();
	void LoadMap(const char *mapName);
	void Update(float deltaTime);
	void AddParticle(Vec4 position, Vec3 velocity);
	void SetupCamera(
		float fov, float width, float height, float nearZ, float farZ
	);
	void SyncCamera(Vec3 position, Vec3 dir);
	void Clear();
	void SetParticleRadius(float radius);

public:
	void Render();
	RendererCompositor compositor;

	explicit Gelly(GellyInitParams &params);
	/**
	 * Initializes Gelly on a separate thread. This is called in the
	 * constructor, but ran on a different thread due to technical limitations.
	 * @param params
	 */
	[[noreturn]] static void InitThreaded(
		Gelly *gelly, const GellyInitParams &params
	);

	void SendGellyMessage(GellyMessage &newMessage);

	~Gelly();
};

#endif	// GELLY_GELLY_H
