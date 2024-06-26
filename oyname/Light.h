#pragma once

#include "gdxutil.h"
#include "gdxdevice.h"
#include "Camera.h"
#include "Transform.h"

using namespace DirectX;

__declspec(align(16))
struct LightBufferData
{
    DirectX::XMFLOAT4 lightPosition;  
    DirectX::XMFLOAT4 lightDirection;
    DirectX::XMFLOAT4 lightDiffuseColor;
    DirectX::XMFLOAT4 lightAmbientColor;
};

class Light
{
public:
    Light();
    ~Light();
    
    void UpdateLight(const gdx::CDevice* device, XMVECTOR position, XMVECTOR lookAt);

    void SetAmbientColor(const DirectX::XMFLOAT4& newColor);
    void SetDiffuseColor(const DirectX::XMFLOAT4& newColor);
    void SetLightType(const D3DLIGHTTYPE lightType);

public:
    ID3D11Buffer*   lightBuffer;
    LightBufferData cbLight;

private:

    D3DLIGHTTYPE    type;
};

