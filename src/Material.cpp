#include "Material.h"

Material::Material() :
    isActive(false),
    m_texture(nullptr),
    m_textureView(nullptr),
    m_imageSamplerState(nullptr),
    m_texture2(nullptr),
    m_textureView2(nullptr),
    m_imageSamplerState2(nullptr),
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
    m_texture = nullptr;
    m_textureView = nullptr;
    m_imageSamplerState = nullptr;

    m_texture2 = nullptr;
    m_textureView2 = nullptr;
    m_imageSamplerState2 = nullptr;

    Memory::SafeRelease(materialBuffer);
    meshes.clear();
}

void Material::SetTexture(const GDXDevice* device)
{
    if (m_textureView && m_imageSamplerState) {
        device->GetDeviceContext()->PSSetShaderResources(0, 1, &m_textureView);
        device->GetDeviceContext()->PSSetSamplers(0, 1, &m_imageSamplerState);
    }
    if (m_textureView2 && m_imageSamplerState2) {
        device->GetDeviceContext()->PSSetShaderResources(1, 1, &m_textureView2);
        device->GetDeviceContext()->PSSetSamplers(1, 1, &m_imageSamplerState2);
    }
}

void Material::SetTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureView, ID3D11SamplerState* sampler)
{
    Memory::SafeRelease(m_imageSamplerState);
    Memory::SafeRelease(m_textureView);
    Memory::SafeRelease(m_texture);

    m_texture = texture;
    m_textureView = textureView;
    m_imageSamplerState = sampler;

    if (m_texture) m_texture->AddRef();
    if (m_textureView) m_textureView->AddRef();
    if (m_imageSamplerState) m_imageSamplerState->AddRef();
}

void Material::SetTexture2(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureView, ID3D11SamplerState* sampler)
{
    Memory::SafeRelease(m_imageSamplerState2);
    Memory::SafeRelease(m_textureView2);
    Memory::SafeRelease(m_texture2);

    m_texture2 = texture;
    m_textureView2 = textureView;
    m_imageSamplerState2 = sampler;

    if (m_texture2) m_texture2->AddRef();
    if (m_textureView2) m_textureView2->AddRef();
    if (m_imageSamplerState2) m_imageSamplerState2->AddRef();
}

void Material::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
    if (materialBuffer != nullptr && context != nullptr) {
        context->UpdateSubresource(materialBuffer, 0, nullptr, &properties, 0, 0);
        context->PSSetConstantBuffers(2, 1, &materialBuffer);
    }
}
