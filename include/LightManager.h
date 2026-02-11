#pragma once

#include <vector>
#include "gdxutil.h"
#include "gdxdevice.h"
#include "Light.h"

using namespace DirectX;

#define MAX_LIGHTS 32

// Array-Buffer für alle Lichter
struct LightArrayBuffer
{
    LightBufferData lights[MAX_LIGHTS];
    unsigned int lightCount;
    DirectX::XMFLOAT3 padding;
};

class LightManager
{
public:
    LightManager();
    ~LightManager();

    // Neue API: Erstelle ein neues Licht mit modernem LightType
    Light* createLight(LightType type);

    // Alte API (Rückwärtskompatibilität): Konvertiert D3DLIGHTTYPE zu LightType
    Light* createLight(D3DLIGHTTYPE type);

    // Aktualisiere alle Lichter und lade sie zur GPU
    void Update(const gdx::CDevice* device);

    // Synchronisiere Licht mit Kamera (Position + Rotation)
    // offset = Optional: Position relativ zur Kamera (z.B. für Schatten)
    void PositionLightAtCamera(Light* light, class Camera* camera,
        DirectX::XMVECTOR offset = DirectX::XMVectorZero());

    // Getter für Lichter-Anzahl
    size_t GetLightCount() const { return m_lights.size(); }
    Light* GetLight(size_t index) const { return (index < m_lights.size()) ? m_lights[index] : nullptr; }

private:
    void InitializeLightBuffer(const gdx::CDevice* device);

    std::vector<Light*> m_lights;
    ID3D11Buffer* lightBuffer;
    LightArrayBuffer lightCBData;
};