#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Transform.h"
#include "gdxutil.h"

namespace gdx {
    class CDevice;
}

class Entity {

public:
    Transform transform;
    MatrixSet matrixSet;
    ID3D11Buffer* constantBuffer;
    D3D11_VIEWPORT viewport;

public:
    Entity();
    virtual ~Entity();

    // Basis-Update für Transform → Matrix
    virtual void Update(const gdx::CDevice* device);

    // Matrix-Generierung für alle Entities
    void GenerateViewMatrix(DirectX::XMVECTOR position,
        DirectX::XMVECTOR lookAt,
        DirectX::XMVECTOR up);

    void GenerateProjectionMatrix(float fieldOfView, float screenAspect,
        float nearZ, float farZ);

    void GenerateViewport(float TopLeftX, float TopLeftY,
        float Width, float Height,
        float MinDepth, float MaxDepth);

    void* operator new(size_t size) {
        return _aligned_malloc(size, 16);
    }
    void operator delete(void* p) noexcept {
        _aligned_free(p);
    }

public:
    bool IsActive() const noexcept { return isActive; }
    void SetActive(bool active) noexcept { isActive = active; }

protected:
    bool isActive;
};

typedef Entity* LPENTITY;
typedef Entity ENTITY;