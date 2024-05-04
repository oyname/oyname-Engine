#include "Brush.h"

Brush::Brush() :
    isActive(false),
    shininess(0.0f),
    transparency(0.0f),
    meshes(nullptr),
    m_texture(nullptr),
    m_textureView(nullptr),
    m_imageSamplerState(nullptr),
    pShader(nullptr)
{
    diffuseColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    specularColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

Brush::~Brush() {

}

void Brush::SetTexture(const gdx::CDevice* device)
{
    device->GetDeviceContext()->PSSetShaderResources(0, 1, &m_textureView);
    device->GetDeviceContext()->PSSetSamplers(0, 1, &m_imageSamplerState);
}

void Brush::SetTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureView, ID3D11SamplerState* imageSamplerState)
{
    m_texture = texture;
    m_textureView = textureView;
    m_imageSamplerState = imageSamplerState;
}
    

