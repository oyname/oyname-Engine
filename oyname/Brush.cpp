#include "Brush.h"

Brush::Brush() :
    isActive(false),
    shininess(0.0f),
    transparency(0.0f),
    pTexture(nullptr),
    meshes(nullptr)
{
    diffuseColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    specularColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

Brush::~Brush() {
    Memory::SafeRelease(pTexture);
}
