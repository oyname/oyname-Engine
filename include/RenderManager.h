#pragma once
#include "ObjectManager.h"
#include "LightManager.h"
#include "RenderQueue.h"
#include <d3d11.h>

class RenderManager {
public:
    RenderManager(ObjectManager& objectManager, LightManager& lightManager);
    ~RenderManager() = default;

    void SetCamera(LPENTITY camera);
    void SetDirectionalLight(LPENTITY dirLight);
    void RenderScene();

    // Phase 4: Shadow Mapping 2-Pass
    void RenderShadowPass();
    void RenderNormalPass();

private:
    RenderQueue m_opaque;
    std::vector<DrawEntry> m_transCandidates; // registriert, aber nicht sortiert
    std::vector<std::pair<float, DrawEntry>> m_transFrame; // pro Frame sortiert

    // Objekte im 3D Raum
    LPENTITY m_currentCam;
    LPENTITY m_directionLight;

    // Manager-Klassen (Referenzen)
    ObjectManager& m_objectManager;
    LightManager& m_lightManager;

    // Helper Functions
    void UpdateShadowMatrixBuffer(const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projMatrix);

    // Default-Konstruktor gelöscht
    RenderManager() = delete;
};
