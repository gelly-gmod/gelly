#include <detail/Camera.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>

Camera::Camera() : view{}, projection{} {
	XMStoreFloat4x4(&view, XMMatrixIdentity());
	XMStoreFloat4x4(&projection, XMMatrixIdentity());
}

// We tranpose all of our matricies before setting them because these are going
// to the GPU directly. The GPU expects the data to be in column-major order,
// but DirectXMath uses row-major order.

void Camera::InvalidateView() {
	// Source engine view calculation, see here:
	// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/game/client/view.cpp#L191

	auto pos = XMLoadFloat3(&eye);
	auto dir = XMLoadFloat3(&direction);
	auto upWorld = XMVectorSet(0.f, 0.f, 1.f, 1.f);
	auto right = XMVector3Cross(dir, upWorld);
	auto up = XMVector3Cross(right, dir);

	// normalize before continuing
	dir = XMVector3Normalize(dir);
	right = XMVector3Normalize(right);
	up = XMVector3Normalize(up);

	XMFLOAT4X4 matCamInverse{};
	XMStoreFloat4x4(&matCamInverse, XMMatrixIdentity());

	for (int i = 0; i < 3; ++i) {
		matCamInverse.m[0][i] = XMVectorGetByIndex(right, i);
		matCamInverse.m[1][i] = XMVectorGetByIndex(up, i);
		matCamInverse.m[2][i] = -XMVectorGetByIndex(dir, i);
		matCamInverse.m[3][i] = 0.0F;
	}

	matCamInverse.m[0][3] = -XMVectorGetX(XMVector3Dot(right, pos));
	matCamInverse.m[1][3] = -XMVectorGetX(XMVector3Dot(up, pos));
	matCamInverse.m[2][3] = XMVectorGetX(XMVector3Dot(dir, pos));
	matCamInverse.m[3][3] = 1.0F;

	XMStoreFloat4x4(&view, (XMLoadFloat4x4(&matCamInverse)));
	XMStoreFloat4x4(
		&invView, XMMatrixInverse(nullptr, XMLoadFloat4x4(&matCamInverse))
	);
}

void Camera::InvalidateProjection() {
	// Source engine projection calculation, see here:
	// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/src_main/hammer/camera.cpp#L343

	XMFLOAT4X4 matProjection{};
	XMStoreFloat4x4(&matProjection, XMMatrixIdentity());

	float w = 2.f * nearZ * tanf(fov * M_PI / 360.f);
	float h = (w * height) / width;

	matProjection.m[0][0] = 2 * nearZ / w;
	matProjection.m[1][1] = 2 * nearZ / h;
	matProjection.m[2][2] = farZ / (nearZ - farZ);
	matProjection.m[2][3] = (nearZ * farZ) / (nearZ - farZ);
	matProjection.m[3][2] = -1;

	XMStoreFloat4x4(&projection, (XMLoadFloat4x4(&matProjection)));
	XMStoreFloat4x4(
		&invProjection, XMMatrixInverse(nullptr, XMLoadFloat4x4(&matProjection))
	);
}

void Camera::SetPosition(float x, float y, float z) {
	eye = {x, y, z};
	InvalidateView();
}

void Camera::SetDirection(float x, float y, float z) {
	direction = {x, y, z};
	InvalidateView();
}

void Camera::SetPerspective(
	float fov, float width, float height, float nearZ, float farZ
) {
	this->fov = fov;
	this->width = width;
	this->height = height;
	this->nearZ = nearZ;
	this->farZ = farZ;

	InvalidateProjection();
}

XMFLOAT4X4 Camera::GetViewMatrix() const { return view; }

XMFLOAT4X4 Camera::GetProjectionMatrix() const { return projection; }

XMFLOAT4X4 Camera::GetInvProjectionMatrix() const { return invProjection; }

XMFLOAT4X4 Camera::GetInvViewMatrix() const { return invView; }