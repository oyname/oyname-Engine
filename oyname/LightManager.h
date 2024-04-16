#pragma once

#include <vector>
#include "gdxutil.h"
#include "Light.h"

class LightManager
{
public:
    LightManager();
    ~LightManager();

    LIGHT* createLight(D3DLIGHTTYPE type);

    void Update(const gdx::CDevice* device);

    std::vector<Light*> m_lights;
private:
};
