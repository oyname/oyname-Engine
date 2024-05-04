#pragma once

#include "gdxutil.h"
#include "Mesh.h"

__declspec(align(16))
struct LightSet
{
    DirectX::XMFLOAT4 lightPosition;  
    DirectX::XMFLOAT4 lightDirection;
    DirectX::XMFLOAT4 lightDiffuseColor;
    DirectX::XMFLOAT4 lightAmbientColor;
};

class Light : public Mesh
{
public:
    Light();
    ~Light();
    
    void UpdateLight(const gdx::CDevice* device); 

    void SetAmbientColor(const DirectX::XMFLOAT4& newColor);
    void SetDiffuseColor(const DirectX::XMFLOAT4& newColor);
    void SetLightType(const D3DLIGHTTYPE lightType);

    const DirectX::XMFLOAT4 GetPosition() const {
        return cbLight.lightPosition;
    }
    const DirectX::XMFLOAT4 GetDirection() const {
        return cbLight.lightDirection;
    }
    const DirectX::XMFLOAT4 GetDiffuseColor() const {
        return cbLight.lightDiffuseColor;
    }
    const DirectX::XMFLOAT4 GetAmbientColor() const {
        return cbLight.lightDiffuseColor;
    }

    void GenerateViewMatrix();
    void GenerateProjectionMatrix(float nearZ, float farZ);

    DirectX::XMMATRIX const GetViewMatrix(DirectX::XMMATRIX&) const {
        return this->cb.viewMatrix;
    }
    DirectX::XMMATRIX const GetProjectionMatrix(DirectX::XMMATRIX&) const {
        return this->cb.projectionMatrix;
    }

    void* operator new(size_t size) {
        // Ausrichtung auf 16 Bytes 
        return _aligned_malloc(size, 16);
    }

    void operator delete(void* p) noexcept {
        _aligned_free(p);
    }

public:
    ID3D11Buffer* lightBuffer;
    LightSet      cbLight;

private:

    D3DLIGHTTYPE    type;

    void SetPosition(const DirectX::XMFLOAT4& newPosition);
    void SetPosition(const DirectX::XMVECTOR& Position);
    void SetDirection(const DirectX::XMFLOAT4& newDirection);
    void SetDirection(const DirectX::XMVECTOR& Direction);
};

typedef Light LIGHT;
typedef Light* LPLIGHT;
