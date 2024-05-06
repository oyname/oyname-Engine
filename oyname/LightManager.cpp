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

ENTITY* LightManager::createLight(D3DLIGHTTYPE type) {
    Mesh* light = new Mesh;
    light->SetLightType(type);
    m_lights.push_back(light);
    return light;
}

void LightManager::Update(const gdx::CDevice* device)
{
    for (const auto& light : m_lights)
    {
        light->UpdateLight(device, light->transform.getPosition(), light->transform.getLookAt());
    }
}
