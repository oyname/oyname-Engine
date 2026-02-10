#include "LightManager.h"
#include "gdxengine.h"
#include "Memory.h"

LightManager::LightManager() : lightBuffer(nullptr)
{
    ZeroMemory(&lightCBData, sizeof(LightArrayBuffer));
}

LightManager::~LightManager()
{
    for (auto& light : m_lights) {
        Memory::SafeDelete(light);
    }
    m_lights.clear();
    Memory::SafeRelease(lightBuffer);
}

Light* LightManager::createLight(D3DLIGHTTYPE type)
{
    if (m_lights.size() >= MAX_LIGHTS) {
        return nullptr;  // Max Lichter erreicht
    }

    Light* light = new Light;
    light->SetLightType(type);
    m_lights.push_back(light);
    return light;
}

void LightManager::InitializeLightBuffer(const gdx::CDevice* device)
{
    if (lightBuffer != nullptr) {
        Memory::SafeRelease(lightBuffer);
    }

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(LightArrayBuffer);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = device->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &lightBuffer);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        return;
    }
}

void LightManager::Update(const gdx::CDevice* device)
{
    // Buffer initialisieren wenn nötig
    if (lightBuffer == nullptr) {
        InitializeLightBuffer(device);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // WICHTIG: Erst ALLE Lichter aktualisieren! 
    // Das berechnet lightDirection aus der Transform-Rotation
    // ═══════════════════════════════════════════════════════════════════════════
    for (auto& light : m_lights) {
        light->Update(device);  // ← KRITISCH! Synchronisiert Transform → lightDirection
    }

    // Hole globales Ambient von der Engine
    DirectX::XMFLOAT4 globalAmbient(0.2f, 0.2f, 0.2f, 1.0f);
    if (gdx::CGIDX::GetInstance() != nullptr) {
        globalAmbient = gdx::CGIDX::GetInstance()->GetGlobalAmbient();
    }

    // Jetzt alle Lichter SAMMELN mit den aktualisierten Daten
    for (size_t i = 0; i < m_lights.size(); ++i)
    {
        auto& light = m_lights[i];

        // Ambient nur für erstes Licht
        DirectX::XMFLOAT4 ambient = (i == 0) ? globalAmbient : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

        // Lichter-Daten direkt in den Array eintragen
        // Jetzt mit AKTUALISIERTEN Werten (von Update() berechnet)
        lightCBData.lights[i].lightPosition = light->cbLight.lightPosition;
        lightCBData.lights[i].lightDirection = light->cbLight.lightDirection;  // ← Jetzt aktualisiert!
        lightCBData.lights[i].lightDiffuseColor = light->cbLight.lightDiffuseColor;
        lightCBData.lights[i].lightAmbientColor = ambient;
    }

    // Lichter-Anzahl setzen
    lightCBData.lightCount = static_cast<unsigned int>(m_lights.size());

    // Nur EINMAL alle Lichter auf einmal hochladen!
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = device->GetDeviceContext()->Map(lightBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        return;
    }

    memcpy(mappedResource.pData, &lightCBData, sizeof(LightArrayBuffer));
    device->GetDeviceContext()->Unmap(lightBuffer, 0);

    // Buffer nur EINMAL an Shader binden (für VS und PS)
    device->GetDeviceContext()->VSSetConstantBuffers(1, 1, &lightBuffer);
    device->GetDeviceContext()->PSSetConstantBuffers(1, 1, &lightBuffer);
}