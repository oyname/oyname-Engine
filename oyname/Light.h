#pragma once

#include "gdxutil.h"
#include "Mesh.h"

__declspec(align(16))
struct LightSet
{
    DirectX::XMFLOAT4 lightPosition;  
    DirectX::XMFLOAT4 lightDirection;
    DirectX::XMFLOAT4 lightColor;
};

class Light : public Mesh
{
public:
    Light();
    ~Light();
    
    void SetColor(const DirectX::XMFLOAT4& newColor);
    
    void SetPosition(const DirectX::XMFLOAT4& newPosition);
    
    void SetPosition(const DirectX::XMVECTOR& Position);
    
    void SetDirection(const DirectX::XMFLOAT4& newDirection);
    
    void SetDirection(const DirectX::XMVECTOR& Direction);
    
    void UpdateLight(const gdx::CDevice* device);   

    DirectX::XMFLOAT4 GetPosition() const;
    DirectX::XMFLOAT4 GetDirection() const;
    DirectX::XMFLOAT4 GetColor() const;

    float GetIntensity();

    LightSet        cbLight;
    D3DLIGHTTYPE    type;
    ID3D11Buffer*   lightBuffer;

    void* operator new(size_t size) {
        // Ausrichtung auf 16 Bytes 
        return _aligned_malloc(size, 16);
    }

    void operator delete(void* p) noexcept {
        _aligned_free(p);
    }
private:

    float m_Intensity;
};

typedef Light LIGHT;
typedef Light* LPLIGHT;
