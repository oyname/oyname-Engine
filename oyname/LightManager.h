#pragma once

#include <vector>
#include "gdxutil.h"
#include "Light.h"
#include "Mesh.h"

using namespace DirectX;

class LightManager
{
public:
    LightManager();
    ~LightManager();

    ENTITY* createLight(D3DLIGHTTYPE type);

    void Update(const gdx::CDevice* device);

    std::vector<Mesh*> m_lights;
private:
};
