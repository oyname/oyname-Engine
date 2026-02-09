#pragma once
#include "Entity.h"
#include "gdxutil.h"
#include "gdxdevice.h"

using namespace DirectX;

__declspec(align(16))
struct LightBufferData
{
    DirectX::XMFLOAT4 lightPosition;
    DirectX::XMFLOAT4 lightDirection;
    DirectX::XMFLOAT4 lightDiffuseColor;
    DirectX::XMFLOAT4 lightAmbientColor;
};

class Light : public Entity
{
public:
    Light();
    ~Light();

    void UpdateLight(const gdx::CDevice* device, XMVECTOR position, XMVECTOR lookAt);
    void SetAmbientColor(const DirectX::XMFLOAT4& newColor);
    void SetDiffuseColor(const DirectX::XMFLOAT4& newColor);
    void SetLightType(D3DLIGHTTYPE lightType);

public:
    ID3D11Buffer* lightBuffer;
    LightBufferData cbLight;
    D3DLIGHTTYPE type;
};

typedef Light* LPLIGHT;