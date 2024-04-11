#pragma once

#include <vector>
#include "gdxutil.h"
#include "Light.h"

class LightManager
{
public:
    LightManager();
    ~LightManager();

    LIGHT* createLight();

    void Update(const gdx::CDevice* device);

    std::vector<Light*> m_lights;
private:

};

