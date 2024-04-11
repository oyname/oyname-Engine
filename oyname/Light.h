#pragma once

#include "gdxutil.h"
#include "Mesh.h"

__declspec(align(16))
struct LightSet
{
    DirectX::XMFLOAT4 lightPosition;  
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
    
    DirectX::XMFLOAT4 GetPosition() const;
    DirectX::XMFLOAT4 GetColor() const;

    float GetIntensity();

    LightSet cbLight;
    ID3D11Buffer* lightBuffer;
    float m_Intensity;

    void UpdateDirectionalLight(const gdx::CDevice* device);

    void* operator new(size_t size) {
        // Ausrichtung auf 16 Bytes 
        return _aligned_malloc(size, 16);
    }

    void operator delete(void* p) noexcept {
        _aligned_free(p);
    }
private:
};

typedef Light LIGHT;
typedef Light* LPLIGHT;

