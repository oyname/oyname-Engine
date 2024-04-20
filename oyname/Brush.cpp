#include "Brush.h"

Brush::Brush() :
    isActive(false),
    shininess(0.0f),
    transparency(0.0f),
    meshes(nullptr)
{
    diffuseColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    specularColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

Brush::~Brush() {

}

void Brush::SetTexture(const gdx::CDevice* device)
{
    device->GetDeviceContext()->PSSetShaderResources(0, 1, &textureView);
    device->GetDeviceContext()->PSSetSamplers(0, 1, &imageSamplerState);
}
