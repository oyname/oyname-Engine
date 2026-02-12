#include "Material.h"

Material::Material() :
    isActive(false),
    m_texture(nullptr),
    m_textureView(nullptr),
    m_imageSamplerState(nullptr),
    materialBuffer(nullptr),
    pRenderShader(nullptr)
{
    // Defaults
    properties.diffuseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    properties.specularColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    properties.shininess = 32.0f;
    properties.transparency = 1.0f;

    // Shadow flags default
    castShadows = true;
    receiveShadows = true;
    properties.receiveShadows = 1.0f;
}

Material::~Material() {
    Memory::SafeRelease(m_texture);
    Memory::SafeRelease(m_textureView);
    Memory::SafeRelease(m_imageSamplerState);
    Memory::SafeRelease(materialBuffer);
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
