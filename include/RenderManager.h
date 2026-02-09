#pragma once
#include "ObjectManager.h"
#include "LightManager.h"

class RenderManager {
public:
    RenderManager(ObjectManager& objectManager, LightManager& lightManager);
    ~RenderManager() = default;
    void SetCamera(LPENTITY camera);
    void SetDirectionalLight(LPENTITY dirLight);
    void RenderScene();
    void RenderShadow(const DirectX::XMMATRIX view, const DirectX::XMMATRIX proj);
private:
    // Objekte im 3D Raum
    LPENTITY m_currentCam;
    LPENTITY m_directionLight;
    // Manager-Klassen (Referenzen)
    ObjectManager& m_objectManager;
    LightManager& m_lightManager;
    // ← Default-Konstruktor gelöscht - Objekte MÜSSEN mit ObjectManager und LightManager erstellt werden
    RenderManager() = delete;
};