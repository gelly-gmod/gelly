#include <detail/Camera.h>

Camera::Camera() :
    view{},
    projection{}
{
    XMStoreFloat4x4(&view, XMMatrixIdentity());
    XMStoreFloat4x4(&projection, XMMatrixIdentity());
}

// We tranpose all of our matricies before setting them because these are going to the GPU directly.
// The GPU expects the data to be in column-major order, but DirectXMath uses row-major order.

void Camera::InvalidateView() {
    XMVECTOR pos = XMLoadFloat3(&position);
    XMVECTOR rot = XMLoadFloat3(&rotation);

    XMStoreFloat4x4(&view, XMMatrixTranspose(XMMatrixRotationRollPitchYawFromVector(rot) * XMMatrixTranslationFromVector(pos)));
}

void Camera::InvalidateProjection() {
    float fov = XMVectorGetX(perspective);
    float aspect = XMVectorGetY(perspective);
    float nearZ = XMVectorGetZ(perspective);
    float farZ = XMVectorGetW(perspective);

    XMStoreFloat4x4(&projection, XMMatrixTranspose(XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ)));
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
    perspective = {fov, aspect, nearZ, farZ};
    InvalidateProjection();
}

// We copy these since we don't want to expose a reference to the internal data.

XMFLOAT4X4 Camera::GetView() const {
    return view;
}

XMFLOAT4X4 Camera::GetProjection() const {
    return projection;
}