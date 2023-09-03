#ifndef GELLY_CAMERA_H
#define GELLY_CAMERA_H

#include <directxmath.h>
using namespace DirectX;

class Camera {
private:
	XMFLOAT4X4 view{};
	XMFLOAT4X4 projection{};
	XMFLOAT4X4 invProjection{};
	XMFLOAT4X4 invView{};

	XMFLOAT3 eye{};
	XMFLOAT3 direction{};

	float fov;
	float width;
	float height;
	float nearZ;
	float farZ;

	void InvalidateView();
	void InvalidateProjection();

public:
	Camera();
	~Camera() = default;

	void SetPosition(float x, float y, float z);
	/**
	 * @note Expected to be the forward vector of the camera.
	 */
	void SetDirection(float x, float y, float z);

	void SetPerspective(
		float fov, float width, float height, float nearZ, float farZ
	);

	[[nodiscard]] XMFLOAT4X4 GetViewMatrix() const;
	[[nodiscard]] XMFLOAT4X4 GetProjectionMatrix() const;
	[[nodiscard]] XMFLOAT4X4 GetInvProjectionMatrix() const;
	[[nodiscard]] XMFLOAT4X4 GetInvViewMatrix() const;
};

#endif	// GELLY_CAMERA_H
