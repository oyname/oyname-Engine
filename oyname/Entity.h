#pragma once

#include <DirectXMath.h>
#include "MeshManager.h"
#include "Object3D.h"
#include "gdxengine.h"

using namespace DirectX;

class Entity
{
public:
    Entity();
    ~Entity();

    void Init(gdx::CGIDX* engine);

    void CreateMesh(LPMESH* mesh, BRUSH* lpBrush);

private:
    gdx::CGIDX* m_engine;
};


