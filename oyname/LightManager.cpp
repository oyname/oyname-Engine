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

LIGHT* LightManager::createLight(D3DLIGHTTYPE type) {
    LIGHT* light = new LIGHT;
    light->SetLightType(type);
    m_lights.push_back(light);
    return light;
}

void LightManager::Update(const gdx::CDevice* device)
{
    for (const auto& light : m_lights)
    {
        light->UpdateLight(device);
        light->Update(); //viewmatrix for shadow
        //light->UpdateConstantBuffer(device, light->cb.viewMatrix, light->cb.projectionMatrix);
    }
}
