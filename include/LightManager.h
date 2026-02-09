#pragma once

#include <vector>
#include "gdxutil.h"
#include "gdxdevice.h"
#include "Light.h"

using namespace DirectX;

class LightManager
{
public:
    LightManager();
    ~LightManager();

    Light* createLight(D3DLIGHTTYPE type);  // ← GEÄNDERT: Gibt Light* zurück

    void Update(const gdx::CDevice* device);

    std::vector<Light*> m_lights;  // ← GEÄNDERT: Light* statt Mesh*
};
