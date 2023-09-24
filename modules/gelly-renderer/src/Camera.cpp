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
	// Old D3DX view matrix calculation:
	// https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatrh

	auto pos = XMLoadFloat3(&eye);
	auto dir = XMLoadFloat3(&direction);
	auto upWorld = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	auto right = XMVector3Cross(dir, upWorld);
	auto up = XMVector3Cross(right, dir);

	XMFLOAT4 zaxis{};
	XMStoreFloat4(
		&zaxis, XMVector3Normalize(XMVectorSubtract(pos, XMVectorAdd(pos, dir)))
	);

	XMFLOAT4 xaxis{};
	XMStoreFloat4(
		&xaxis, XMVector3Normalize(XMVector3Cross(up, XMLoadFloat4(&zaxis)))
	);

	XMFLOAT4 yaxis{};
	XMStoreFloat4(
		&yaxis, XMVector3Cross(XMLoadFloat4(&zaxis), XMLoadFloat4(&xaxis))
	);

	XMFLOAT4X4 matCam{};
	XMStoreFloat4x4(&matCam, XMMatrixIdentity());

	matCam.m[0][0] = xaxis.x;
	matCam.m[1][0] = xaxis.y;
	matCam.m[2][0] = xaxis.z;

	matCam.m[0][1] = yaxis.x;
	matCam.m[1][1] = yaxis.y;
	matCam.m[2][1] = yaxis.z;

	matCam.m[0][2] = zaxis.x;
	matCam.m[1][2] = zaxis.y;
	matCam.m[2][2] = zaxis.z;

	matCam.m[3][0] = -XMVectorGetX(XMVector3Dot(XMLoadFloat4(&xaxis), pos));
	matCam.m[3][1] = -XMVectorGetX(XMVector3Dot(XMLoadFloat4(&yaxis), pos));
	matCam.m[3][2] = -XMVectorGetX(XMVector3Dot(XMLoadFloat4(&zaxis), pos));
	matCam.m[3][3] = 1.f;

	XMStoreFloat4x4(&view, XMMatrixTranspose(XMLoadFloat4x4(&matCam)));
	XMStoreFloat4x4(
		&invView,
		XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&matCam)))
	);
}

void Camera::InvalidateProjection() {
	// w and h are the width and height of near plane
	float fovRadians = fov * (float)M_PI / 360.f;
	float w = 2.f * nearZ * tanf(fovRadians);
	float h = (w * height) / width;

	XMStoreFloat4x4(
		&projection, XMMatrixPerspectiveFovRH(fovRadians, w / h, nearZ, farZ)
	);
	XMStoreFloat4x4(
		&invProjection, XMMatrixInverse(nullptr, XMLoadFloat4x4(&projection))
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

XMFLOAT3 Camera::GetPosition() const { return eye; }

float Camera::GetFOV() const { return fov; }
