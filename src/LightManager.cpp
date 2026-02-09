#include "LightManager.h"
#include "gdxengine.h"  // Für Zugriff auf globales Ambient

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

Light* LightManager::createLight(D3DLIGHTTYPE type)
{
    Light* light = new Light;  // ← GEÄNDERT: Light statt Mesh
    light->SetLightType(type);
    m_lights.push_back(light);
    return light;
}
void LightManager::Update(const gdx::CDevice* device)
{
    // Hole globales Ambient von der Engine
    DirectX::XMFLOAT4 globalAmbient(0.2f, 0.2f, 0.2f, 1.0f);
    if (gdx::CGIDX::GetInstance() != nullptr) {
        globalAmbient = gdx::CGIDX::GetInstance()->GetGlobalAmbient();
    }

    for (size_t i = 0; i < m_lights.size(); ++i)
    {
        auto& light = m_lights[i];

        // Nur das erste Licht trägt das globale Ambient
        if (i == 0) {
            light->cbLight.lightAmbientColor = globalAmbient;
        }
        else {
            light->cbLight.lightAmbientColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        }

        light->UpdateLight(device, light->transform.getPosition(), light->transform.getLookAt());
    }
}
