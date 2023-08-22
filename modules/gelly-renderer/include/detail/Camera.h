#ifndef GELLY_CAMERA_H
#define GELLY_CAMERA_H

#include <directxmath.h>
using namespace DirectX;

class Camera {
private:
    XMFLOAT4X4 view{};
    XMFLOAT4X4 projection{};
    XMFLOAT3 position{};
    XMFLOAT3 rotation{};
    /**
     * The x component is the field of view in radians.
     * The y component is the aspect ratio.
     * The z component is the near plane.
     * The w component is the far plane.
     */
    XMVECTOR perspective{};

    void InvalidateView();
    void InvalidateProjection();
public:
    Camera();
    ~Camera() = default;

    void SetPosition(float x, float y, float z);
    /**
     * @note The given angles are expected to be in radians.
     * @param x Rotation around the x axis.
     * @param y Rotation around the y axis.
     * @param z Rotation around the z axis.
     */
    void SetRotation(float x, float y, float z);

    void SetPerspective(float fov, float aspect, float nearZ, float farZ);

    [[nodiscard]] XMFLOAT4X4 GetVPMatrix() const;
};

#endif //GELLY_CAMERA_H
