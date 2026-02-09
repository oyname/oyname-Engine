#pragma once
#include "Entity.h"
#include "gdxutil.h"

using namespace DirectX;

class Camera : public Entity
{
public:
    enum PROJ_MODE {
        ORTHO_LH,
        PERSPECTIVE_LH,
    };

    struct Perspective {
        float fieldOfView;
        float aspectRatio;
        float nearPlane;
        float farPlane;
    };

public:
    Camera();
    ~Camera();

    // UpdateCamera ist Camera-spezifisch
    void UpdateCamera(XMVECTOR position, XMVECTOR direction, XMVECTOR up);
};

typedef Camera* LPCAMERA;