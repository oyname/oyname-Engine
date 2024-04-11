#include "ObjectManager.h"

ObjectManager::ObjectManager() : m_device(nullptr){ 

}

ObjectManager::~ObjectManager()
{
    // Iterate through all surfaces
    for (auto& surface : m_surfaces) {
        Memory::SafeDelete(surface);
    }
    m_surfaces.clear();

    for (auto& mesh : m_meshes) {
        Memory::SafeDelete(mesh);
    }
    m_meshes.clear();

    for (auto& brush : m_brushes) {
        Memory::SafeDelete(brush);
    }
    m_brushes.clear();

    for (auto& shader : m_shaders) {
        Memory::SafeDelete(shader);
    }
    m_shaders.clear();
}

void ObjectManager::Init(gdx::CDevice* device)
{
    m_device = device;
}

SURFACE* ObjectManager::createSurface() {
    SURFACE* surface = new SURFACE;
    m_surfaces.push_back(surface);
    return surface;
}

MESH* ObjectManager::createMesh() {
    MESH* mesh = new MESH;
    mesh->surfaces = new std::list<SURFACE*>;
    m_meshes.push_back(mesh);
    return mesh;
}

BRUSH* ObjectManager::createBrush() {
    BRUSH* brush = new BRUSH;
    brush->meshes = new std::list<MESH*>;
    m_brushes.push_back(brush);
    return brush;
}

SHADER* ObjectManager::createShader() {
    SHADER* shader = new SHADER;
    shader->brushes = new std::list<BRUSH*>;
    m_shaders.push_back(shader);
    return shader;
}

void ObjectManager::addSurfaceToMesh(MESH* mesh, SURFACE* surface) {
    surface->pShader = mesh->pShader;
    mesh->surfaces->push_back(surface);
}

void ObjectManager::addMeshToBrush(BRUSH* brush, MESH* mesh) {
    mesh->pShader = brush->pShader;
    brush->meshes->push_back(mesh);
}

void ObjectManager::addBrushToShader(SHADER* shader, BRUSH* brush) {
    brush->pShader = shader;
    shader->brushes->push_back(brush);
}

void ObjectManager::deleteSurface(SURFACE* surface) {

    // Iterate through all meshes
    for (auto& mesh : m_meshes) {
        // Iterate through all surfaces in the current mesh
        auto& surfaces = *(mesh->surfaces);
        for (auto it = surfaces.begin(); it != surfaces.end(); ++it) {
            if (*it == surface) {
                // Surface found, remove from the list
                surfaces.erase(it);
                break; // since the surface only occurs once in a mesh
            }
        }
    }

    // Free memory
    auto it = std::find(m_surfaces.begin(), m_surfaces.end(), surface);
    if (it != m_surfaces.end()) {
        m_surfaces.erase(it);
        Memory::SafeDelete(surface);
    }
}

void ObjectManager::deleteMesh(MESH* mesh) {
    // Iterate through all brushes
    for (auto& brush : m_brushes) {
        // Iterate through all meshes in the current brush
        auto& meshes = *(brush->meshes);
        for (auto it = meshes.begin(); it != meshes.end(); ++it) {
            if (*it == mesh) {
                // Mesh found, remove from the list
                meshes.erase(it);
                break;
            }
        }
    }

    // Free memory
    auto it = std::find(m_meshes.begin(), m_meshes.end(), mesh);
    if (it != m_meshes.end()) {
        m_meshes.erase(it);
        Memory::SafeDelete(mesh);
    }
}

void ObjectManager::deleteBrush(BRUSH* brush) {
    // Iterate through all brushes
    for (auto& shader : m_shaders) {
        // Iterate through all brushes in the same shader
        auto& brushes = *(shader->brushes);
        for (auto it = brushes.begin(); it != brushes.end(); ++it) {
            if (*it == brush) {
                // Brush found, remove from the list
                brushes.erase(it);
                break;
            }
        }
    }

    // Free memory
    auto it = std::find(m_brushes.begin(), m_brushes.end(), brush);
    if (it != m_brushes.end()) {
        m_brushes.erase(it);
        Memory::SafeDelete(brush);
    }
}

void ObjectManager::removeSurfacefromMesh(MESH* mesh, SURFACE* surface) {
    // Iterate through all brushes

    auto& surfaces = *(mesh->surfaces);
    for (auto it = surfaces.begin(); it != surfaces.end(); ++it) {
        if (*it == surface) {
            // Surface found, remove from the list
            surfaces.erase(it);
            break;
        }
    }
}

void ObjectManager::removeMeshfromBrush(BRUSH* brush, MESH* mesh) {
    // Iterate through all brushes

    auto& meshes = *(brush->meshes);
    for (auto it = meshes.begin(); it != meshes.end(); ++it) {
        if (*it == mesh) {
            // Mesh found, remove from the list
            meshes.erase(it);
            break;
        }
    }
}

void ObjectManager::removeBrushfromShader(SHADER* shader, BRUSH* brush) {
    // Iterate through all brushes

    auto& brushes = *(shader->brushes);
    for (auto it = brushes.begin(); it != brushes.end(); ++it) {
        if (*it == brush) {
            // Brush found, remove from the list
            brushes.erase(it);
            break;
        }
    }
}

SURFACE* ObjectManager::getPreviousSurface(SURFACE* currentSurface) {
    for (auto it = m_surfaces.begin(); it != m_surfaces.end(); ++it) {
        if (*it == currentSurface) {
            if (it != m_surfaces.begin()) {
                auto prevIt = std::prev(it);
                return *prevIt;
            }
            else {
                return nullptr;
            }
        }
    }
    return nullptr;
}

MESH* ObjectManager::getPreviousMesh(MESH* currentMesh) {
    for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it) {
        if (*it == currentMesh) {
            if (it != m_meshes.begin()) {
                auto prevIt = std::prev(it);
                return *prevIt;
            }
            else {
                return nullptr;
            }
        }
    }
    return nullptr;
}

BRUSH* ObjectManager::getPreviousBrush(BRUSH* currentBrush) {
    for (auto it = m_brushes.begin(); it != m_brushes.end(); ++it) {
        if (*it == currentBrush) {
            if (it != m_brushes.begin()) {
                auto prevIt = std::prev(it);
                return *prevIt;
            }
            else {
                return nullptr;
            }
        }
    }
    return nullptr;
}

// Function to access the previous element in m_shaders
SHADER* ObjectManager::getPreviousShader(SHADER* currentShader)
{
    for (auto it = m_shaders.begin(); it != m_shaders.end(); ++it) {
        if (*it == currentShader) {
            if (it != m_shaders.begin()) {
                auto prevIt = std::prev(it);
                return *prevIt;
            }
            else {
                return nullptr;
            }
        }
    }
    return nullptr; // Case when the passed shader object is not found in the list
}


SHADER* ObjectManager::getShader(SURFACE surface)
{
    return surface.pShader;
}

SHADER* ObjectManager::getShader(MESH mesh)
{
    return mesh.pShader;
}

SHADER* ObjectManager::getShader(BRUSH brush)
{
    return brush.pShader;
}
