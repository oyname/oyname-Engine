#include "Light.h"
#include "Memory.h"

Light::Light() : Entity()  // ← Basis-Konstruktor
{
    lightBuffer = nullptr;
    type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;

    DirectX::XMStoreFloat4(&cbLight.lightPosition,
        DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 0.0f });
    DirectX::XMStoreFloat4(&cbLight.lightDirection,
        DirectX::XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f });

    SetDiffuseColor(DirectX::XMFLOAT4(1.0f, 0.8f, 1.0f, 1.0f));
}

Light::~Light()
{
    Memory::SafeRelease(lightBuffer);
    // constantBuffer wird in Entity::~Entity() freigegeben
}

void Light::SetDiffuseColor(const DirectX::XMFLOAT4& Color)
{
    cbLight.lightDiffuseColor = Color;
}

void Light::SetAmbientColor(const DirectX::XMFLOAT4& Color)
{
    cbLight.lightAmbientColor = Color;
}

void Light::SetLightType(D3DLIGHTTYPE lightType)
{
    this->type = lightType;
}

void Light::UpdateLight(const gdx::CDevice* device, XMVECTOR position, XMVECTOR lookAt)
{
    HRESULT hr = S_OK;

    // Aktualisieren der Position des Lichts
    DirectX::XMStoreFloat4(&cbLight.lightPosition, position);
    DirectX::XMStoreFloat4(&cbLight.lightDirection, lookAt);

    // Kopieren der Daten in den lightBuffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    hr = device->GetDeviceContext()->Map(lightBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0,
        &mappedResource);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        return;
    }

    memcpy(mappedResource.pData, &cbLight, sizeof(LightBufferData));
    device->GetDeviceContext()->Unmap(lightBuffer, 0);

    // Setzen des lightBuffers im Shader
    device->GetDeviceContext()->VSSetConstantBuffers(1, 1, &lightBuffer);
    device->GetDeviceContext()->PSSetConstantBuffers(1, 1, &lightBuffer);
}