#pragma once

#include <vector>
#include "gdxutil.h"
#include "gdxdevice.h"
#include "Light.h"

using namespace DirectX;

#define MAX_LIGHTS 32

// ═══════════════════════════════════════════════════════════════
// Array-Buffer für alle Lichter
// Nutzt die BESTEHENDE LightBufferData Struktur aus Light.h!
// ═══════════════════════════════════════════════════════════════
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

    Light* createLight(D3DLIGHTTYPE type);
    void Update(const gdx::CDevice* device);

private:
    void InitializeLightBuffer(const gdx::CDevice* device);

    std::vector<Light*> m_lights;
    ID3D11Buffer* lightBuffer;
    LightArrayBuffer lightCBData;
};