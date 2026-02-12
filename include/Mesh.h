#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "Entity.h"
#include "gdxutil.h"
#include "Surface.h"

class Material;

enum COLLISION {
    NONE = 0,
    BOX = 1,
    SPHERE = 2,
};

class Mesh : public Entity
{
public:
    Mesh();
    ~Mesh();

    // 1. Überschreibt Entity::Update() - für einfaches Update
    void Update(const gdx::CDevice* device) override;

    // 2. Rendering-spezifisches Update mit MatrixSet
    void Update(const gdx::CDevice* device, MatrixSet* matrixSet);

    unsigned int NumSurface();
    Surface* GetSurface(unsigned int index);
    void AddSurfaceToMesh(Surface* surface);

    void SetCollisionMode(COLLISION collision);
    bool CheckCollision(Mesh* mesh);
    void CalculateOBB(unsigned int index);

    void* operator new(size_t size) {
        return _aligned_malloc(size, 16);
    }
    void operator delete(void* p) noexcept {
        _aligned_free(p);
    }

public:
    std::vector<Surface*> surfaces;
    Material* pMaterial;
    DirectX::BoundingOrientedBox obb;

private:
    COLLISION collisionType;
};

typedef Mesh* LPMESH;
