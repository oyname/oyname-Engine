#include "Material.h"

Material::Material() :
    isActive(false),
    m_texture(nullptr),
    m_textureView(nullptr),
    m_imageSamplerState(nullptr),
    materialBuffer(nullptr),
    pRenderShader(nullptr)
{
    // Initialisiere MaterialData Struct mit vernünftigen Defaults
    properties.diffuseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);    // ← WEISS
    properties.specularColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);   // ← WEISS
    properties.shininess = 32.0f;    // ← Anständiger Glanzgrad
    properties.transparency = 1.0f;  // ← Vollständig opak

    // meshes ist ein vector - keine Initialisierung nötig
}

Material::~Material() {
    // Release GPU-Ressourcen
    Memory::SafeRelease(m_texture);
    Memory::SafeRelease(m_textureView);
    Memory::SafeRelease(m_imageSamplerState);
    Memory::SafeRelease(materialBuffer);

    // meshes Vector wird automatisch aufgeräumt
    // Die Mesh-Objekte selbst werden vom ObjectManager verwaltet, nicht hier löschen!
    meshes.clear();
}

void Material::SetTexture(const gdx::CDevice* device)
{
    if (m_textureView && m_imageSamplerState) {
        device->GetDeviceContext()->PSSetShaderResources(0, 1, &m_textureView);
        device->GetDeviceContext()->PSSetSamplers(0, 1, &m_imageSamplerState);
    }
}

void Material::SetTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureView, ID3D11SamplerState* imageSamplerState)
{
    m_texture = texture;
    m_textureView = textureView;
    m_imageSamplerState = imageSamplerState;
}

void Material::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
    if (materialBuffer != nullptr && context != nullptr) {
        context->UpdateSubresource(materialBuffer, 0, nullptr, &properties, 0, 0);
        context->PSSetConstantBuffers(2, 1, &materialBuffer);
    }
}