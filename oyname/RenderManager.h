#pragma once

#include "MeshManager.h"

class RenderManager {
public:
    RenderManager();
    RenderManager(MeshManager& meshManager);

    void RenderLoop();
    void RenderMesh();
    void UpdateWorld();

private:

    MeshManager& m_meshManager; // Referenz auf den MeshManager
};