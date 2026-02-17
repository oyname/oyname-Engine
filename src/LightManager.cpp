#include "LightManager.h"
#include "gdxengine.h"
#include "Camera.h"
#include "Memory.h"
using namespace DirectX;

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

// Konvertiere D3DLIGHTTYPE zu LightType (für Rückwärtskompatibilität)
static LightType ConvertD3DLightTypeToLightType(D3DLIGHTTYPE d3dType)
{
    // D3DLIGHT_DIRECTIONAL = 1, D3DLIGHT_POINT = 2, D3DLIGHT_SPOT = 3
    if (d3dType == D3DLIGHT_POINT) {
        return LightType::Point;
    }
    // Alles andere ist Directional (DEFAULT)
    return LightType::Directional;
}

// Alte Signatur für Rückwärtskompatibilität (falls noch irgendwo genutzt)
Light* LightManager::CreateLight(D3DLIGHTTYPE type)
{
    // Konvertiere alte API zu neuer API
    LightType lightType = ConvertD3DLightTypeToLightType(type);
    return CreateLight(lightType);
}

Light* LightManager::CreateLight(LightType type)
{
    if (m_lights.size() >= MAX_LIGHTS) {
        Debug::Log("LightManager.cpp: WARNING - Max light count (32) reached, cannot create new light");
        return nullptr;
    }

    Light* light = new Light;
    light->SetLightType(type);

    // Setze Default-Radius für Point-Lichter
    if (type == LightType::Point) {
        light->SetRadius(100.0f);  // 100 Einheiten Standard-Reichweite
    }

    m_lights.push_back(light);
    Debug::Log("LightManager.cpp: Light created successfully (Total lights: ", static_cast<int>(m_lights.size()), ")");
    return light;
}

// Licht an Kamera synchronisieren
void LightManager::PositionLightAtCamera(Light* light, Camera* camera, DirectX::XMVECTOR offset)
{
    if (light == nullptr || camera == nullptr) {
        Debug::Log("LightManager.cpp: ERROR - PositionLightAtCamera received nullptr for light or camera");
        return;
    }

    // 1. Position: Kamera-Position + Offset
    DirectX::XMVECTOR camPos = camera->transform.GetPosition();
    DirectX::XMVECTOR lightPos = DirectX::XMVectorAdd(camPos, offset);

    // 2. Rotation: Exakte Kopie der Kamera-Rotation (Quaternion)
    DirectX::XMVECTOR camRotation = camera->transform.GetRotationQuaternion();

    // 3. Setze Licht mit Position UND Rotation
    light->transform.SetPosition(lightPos);
    light->transform.SetRotationQuaternion(camRotation);

    Debug::Log("LightManager.cpp: Light positioned at camera (Type: ",
        (light->GetLightType() == LightType::Directional ? "Directional" : "Point"), ")");
}

void LightManager::InitializeLightBuffer(const GDXDevice* device)
{
    if (lightBuffer != nullptr) {
        Memory::SafeRelease(lightBuffer);
    }

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(LightArrayBuffer);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  // ← KORRIGIERT: war D3D11_ACCESS_WRITE

    HRESULT hr = device->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &lightBuffer);
    if (FAILED(hr))
    {
        Debug::LogHr(__FILE__, __LINE__, hr);
        return;
    }

    Debug::Log("LightManager.cpp: Light buffer initialized successfully (Capacity: 32 lights)");
}

void LightManager::Update(const GDXDevice* device)
{
    // Buffer initialisieren wenn nötig
    if (lightBuffer == nullptr) {
        InitializeLightBuffer(device);
    }

    // WICHTIG: Erst ALLE Lichter aktualisieren! 
    // Das berechnet lightDirection aus der Transform-Rotation und setzt lightPosition.w korrekt
    for (auto& light : m_lights) {
        light->Update(device);
    }

    // Hole globales Ambient von der Engine
    DirectX::XMFLOAT4 globalAmbient(0.2f, 0.2f, 0.2f, 1.0f);
    if (GDXEngine::GetInstance() != nullptr) {
        globalAmbient = GDXEngine::GetInstance()->GetGlobalAmbient();
    }

    // Jetzt alle Lichter SAMMELN mit den aktualisierten Daten
    for (size_t i = 0; i < m_lights.size(); ++i)
    {
        auto& light = m_lights[i];

        // Ambient nur für erstes Licht
        DirectX::XMFLOAT4 ambient = (i == 0) ? globalAmbient : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

        // Lichter-Daten direkt in den Array eintragen
        lightCBData.lights[i].lightPosition = light->cbLight.lightPosition;     // W ist jetzt korrekt gesetzt
        lightCBData.lights[i].lightDirection = light->cbLight.lightDirection;
        lightCBData.lights[i].lightDiffuseColor = light->cbLight.lightDiffuseColor;  // A ist jetzt Radius
        lightCBData.lights[i].lightAmbientColor = ambient;
    }

    // Lichter-Anzahl setzen
    lightCBData.lightCount = static_cast<unsigned int>(m_lights.size());

    // Nur EINMAL alle Lichter auf einmal hochladen!
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = device->GetDeviceContext()->Map(lightBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(hr))
    {
        Debug::LogHr(__FILE__, __LINE__, hr);
        return;
    }

    memcpy(mappedResource.pData, &lightCBData, sizeof(LightArrayBuffer));
    device->GetDeviceContext()->Unmap(lightBuffer, 0);

    // Buffer nur EINMAL an Shader binden (für VS und PS)
    device->GetDeviceContext()->VSSetConstantBuffers(1, 1, &lightBuffer);
    device->GetDeviceContext()->PSSetConstantBuffers(1, 1, &lightBuffer);
}