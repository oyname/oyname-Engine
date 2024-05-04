#pragma once

#include "ObjectManager.h"
#include "LightManager.h"

class RenderManager {
public:
    RenderManager();
    RenderManager(ObjectManager& objectManager, LightManager& lightManager);

    void SetCamera(LPENTITY camera);
    void SetDirectionalLight(LPLIGHT dirLight);
    void RenderScene(const DirectX::XMMATRIX view, const DirectX::XMMATRIX proj);
    void RenderShadow(const DirectX::XMMATRIX view, const DirectX::XMMATRIX proj);

private:
    // Objekte im 3D Raum
    LPENTITY m_currentCam;
    LPLIGHT m_directionLight;

    // Manager-Klassen
    ObjectManager& m_objectManager; // Referenz auf den ObjektManager
    LightManager& m_lightManager; // Referenz auf den LightManager
};