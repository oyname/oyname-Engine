#pragma once

#include "ObjectManager.h"

class RenderManager {
public:
    RenderManager();
    RenderManager(ObjectManager& objectManager);

    void RenderLoop();
    void RenderMesh();

private:

    ObjectManager& m_objectManager; // Referenz auf den ObjectManager
};