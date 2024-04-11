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

    std::vector<Light*> m_lights;
private:

};

