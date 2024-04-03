#pragma once

#include <DirectXMath.h>
#include "MeshManager.h"

using namespace DirectX;

class Entity 
{
public:
    Entity();
    ~Entity();

    void Rotate(LPMESH mesh, float fRotateX, float fRotateY, float fRotateZ);


private:

};


