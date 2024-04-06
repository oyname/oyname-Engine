#pragma once

#include "ObjectManager.h"

class RenderManager {
public:
    RenderManager();
    RenderManager(ObjectManager& objectManager);

    void SetCamera(LPMESH camera);
    void RenderLoop();
    void RenderMesh();

private:
    LPMESH m_currentCam;
    ObjectManager& m_objectManager; // Referenz auf den MeshManager
};