#include "LightManager.h"

LightManager::LightManager()
{

}

LightManager::~LightManager()
{
    for (auto& light : m_lights) {
        Memory::SafeDelete(light);
    }
    m_lights.clear();
}

LIGHT* LightManager::createLight() {
    LIGHT* light = new LIGHT;
    m_lights.push_back(light);
    return light;
}

void LightManager::Update(const gdx::CDevice* device)
{
    for (const auto& light : m_lights)
    {
        Debug::Log("UPDATEN LIGHT :", light);

        light->UpdateDirectionalLight(device);
    }
}
