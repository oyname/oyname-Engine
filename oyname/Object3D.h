#pragma once

#include <DirectXMath.h>
#include "MeshManager.h"
#include "gdxengine.h"

using namespace DirectX;

class Object3D
{
public:
    Object3D();
    ~Object3D();

    void Init(gdx::CGIDX* engine);
    void PositionEntity(LPMESH mesh, float x, float y, float z);
    void MoveEntity(LPMESH mesh, float x, float y, float z);
    void RotateEntity(LPMESH mesh, float fRotateX, float fRotateY, float fRotateZ);
    void TurnEntity(LPMESH mesh, float fRotateX, float fRotateY, float fRotateZ);

private:
    gdx::CGIDX* m_engine;
};

