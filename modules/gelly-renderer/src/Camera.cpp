#include <detail/Camera.h>

Camera::Camera() : view{}, projection{} {
	XMStoreFloat4x4(&view, XMMatrixIdentity());
	XMStoreFloat4x4(&projection, XMMatrixIdentity());
}

// We tranpose all of our matricies before setting them because these are going
// to the GPU directly. The GPU expects the data to be in column-major order,
// but DirectXMath uses row-major order.

void Camera::InvalidateView() {
	auto pos = XMLoadFloat3(&position);
	auto rot = XMLoadFloat3(&rotation);

	XMStoreFloat4x4(
		&view,
		XMMatrixTranspose(
			XMMatrixTranslationFromVector(pos) *
			XMMatrixRotationRollPitchYawFromVector(rot)
		)
	);
}

void Camera::InvalidateProjection() {
	float fov = XMVectorGetX(perspective);
	float aspect = XMVectorGetY(perspective);
	float nearZ = XMVectorGetZ(perspective);
	float farZ = XMVectorGetW(perspective);

	XMStoreFloat4x4(
		&projection,
		XMMatrixTranspose(XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ))
	);
}

void Camera::SetPosition(float x, float y, float z) {
	position = {x, y, z};
	InvalidateView();
}

void Camera::SetRotation(float x, float y, float z) {
	rotation = {x, y, z};
	InvalidateView();
}

void Camera::SetPerspective(float fov, float aspect, float nearZ, float farZ) {
	perspective = {XMConvertToRadians(fov), aspect, nearZ, farZ};
	InvalidateProjection();
}

XMFLOAT4X4 Camera::GetViewMatrix() const { return view; }

XMFLOAT4X4 Camera::GetProjectionMatrix() const { return projection; }