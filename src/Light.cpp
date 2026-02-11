#include "Light.h"
#include "Memory.h"

Light::Light() : Entity(), lightType(LightType::Directional)
{
    lightBuffer = nullptr;

    // Initialisiere Position mit W=0 (direktionales Licht)
    DirectX::XMStoreFloat4(&cbLight.lightPosition,
        DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 0.0f });

    // Initialisiere Direction
    DirectX::XMStoreFloat4(&cbLight.lightDirection,
        DirectX::XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f });

    // Setze Diffuse mit Default-Radius (100 Einheiten)
    SetDiffuseColor(DirectX::XMFLOAT4(1.0f, 0.8f, 1.0f, 100.0f));  // A = Radius

    // Setze Ambient
    SetAmbientColor(DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f));
}

Light::~Light()
{
    Memory::SafeRelease(lightBuffer);
}

// WICHTIG: Update() überschreiben - berechnet lightDirection 
// automatisch aus der Transform-Rotation
void Light::Update(const gdx::CDevice* device)
{
    // Basis-Update aufrufen (Matrix-Berechnungen)
    Entity::Update(device);

    // lightDirection aus der aktuellen Transform-Rotation berechnen
    // LookAt-Vektor ist die Forward-Richtung nach der Rotation
    XMVECTOR direction = transform.getLookAt();
    DirectX::XMStoreFloat4(&cbLight.lightDirection, direction);

    // Aktualisiere lightPosition basierend auf Licht-Typ
    XMVECTOR pos = transform.getPosition();

    if (lightType == LightType::Directional)
    {
        // Für direktionale Lichter: W=0 (keine Position-Berechnung nötig)
        DirectX::XMFLOAT4 posFloat;
        DirectX::XMStoreFloat4(&posFloat, pos);
        cbLight.lightPosition = DirectX::XMFLOAT4(posFloat.x, posFloat.y, posFloat.z, 0.0f);
    }
    else if (lightType == LightType::Point)
    {
        // Für Point-Lichter: W=1 (Position wird im Shader genutzt)
        DirectX::XMFLOAT4 posFloat;
        DirectX::XMStoreFloat4(&posFloat, pos);
        cbLight.lightPosition = DirectX::XMFLOAT4(posFloat.x, posFloat.y, posFloat.z, 1.0f);
    }
}

void Light::SetDiffuseColor(const DirectX::XMFLOAT4& Color)
{
    // Behalte den Radius (W-Komponente) wenn bereits gesetzt
    float radius = cbLight.lightDiffuseColor.w;
    cbLight.lightDiffuseColor = DirectX::XMFLOAT4(Color.x, Color.y, Color.z, radius);
}

void Light::SetAmbientColor(const DirectX::XMFLOAT4& Color)
{
    cbLight.lightAmbientColor = Color;
}

// Neue API: LightType
void Light::SetLightType(LightType type)
{
    this->lightType = type;
}

// Alte API: D3DLIGHTTYPE (Rückwärtskompatibilität)
void Light::SetLightType(D3DLIGHTTYPE d3dType)
{
    // Konvertiere D3DLIGHTTYPE zu LightType
    // D3DLIGHT_DIRECTIONAL = 1, D3DLIGHT_POINT = 2, D3DLIGHT_SPOT = 3
    if (d3dType == D3DLIGHT_POINT) {
        lightType = LightType::Point;
    }
    else {
        lightType = LightType::Directional;  // Default für alles andere
    }
}

void Light::SetRadius(float radius)
{
    // Aktualisiere nur die A-Komponente (Radius)
    cbLight.lightDiffuseColor.w = radius;
}

void Light::UpdateLight(const gdx::CDevice* device, XMVECTOR position, XMVECTOR lookAt)
{
    HRESULT hr = S_OK;

    // Aktualisiere Position basierend auf Licht-Typ
    DirectX::XMFLOAT4 posFloat;
    DirectX::XMStoreFloat4(&posFloat, position);

    if (lightType == LightType::Directional)
    {
        cbLight.lightPosition = DirectX::XMFLOAT4(posFloat.x, posFloat.y, posFloat.z, 0.0f);
    }
    else
    {
        cbLight.lightPosition = DirectX::XMFLOAT4(posFloat.x, posFloat.y, posFloat.z, 1.0f);
    }

    // Aktualisiere Direction
    DirectX::XMStoreFloat4(&cbLight.lightDirection, lookAt);

    // Kopiere die Daten in den lightBuffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    hr = device->GetDeviceContext()->Map(lightBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0,
        &mappedResource);

    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        Debug::Log("Light.cpp: Failed to map light buffer");
        return;
    }

    memcpy(mappedResource.pData, &cbLight, sizeof(LightBufferData));
    device->GetDeviceContext()->Unmap(lightBuffer, 0);

    // Setze den lightBuffer im Shader
    device->GetDeviceContext()->VSSetConstantBuffers(1, 1, &lightBuffer);
    device->GetDeviceContext()->PSSetConstantBuffers(1, 1, &lightBuffer);
}