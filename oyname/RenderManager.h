#pragma once

#include "ObjectManager.h"
#include "LightManager.h"

class RenderManager {
public:
    RenderManager();
    RenderManager(ObjectManager& objectManager, LightManager& lightManager);

    void SetCamera(LPMESH camera);
    void SetDirectionalLight(LPLIGHT dirLight);
    void RenderLoop();
    void RenderMesh();

private:
    // Objekte im 3D Raum
    LPMESH m_currentCam;
    LPLIGHT m_directionLight;

    // Manager-Klassen
    ObjectManager& m_objectManager; // Referenz auf den ObjektManager
    LightManager& m_lightManager; // Referenz auf den LightManager
};