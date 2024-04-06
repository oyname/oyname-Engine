#include "gdxengine.h"
#include "MeshManager.h"

MeshManager::MeshManager() : m_device(nullptr)
{

}

MeshManager::~MeshManager() 
{
    // Durchlaufen aller Surfaces
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

void MeshManager::Init(gdx::CDevice* device, gdx::CGIDX* engine)
{
    m_engine = engine;
    m_device = device;
}

SURFACE* MeshManager::createSurface() {
    SURFACE* surface = new SURFACE;
    m_surfaces.push_back(surface);
    return surface;
}

MESH* MeshManager::createMesh() {
    MESH* mesh = new MESH;
    mesh->surfaces = new std::list<SURFACE*>;
    m_meshes.push_back(mesh);
    return mesh;
}

BRUSH* MeshManager::createBrush() {
    BRUSH* brush = new BRUSH;
    brush->meshes = new std::list<MESH*>;
    m_brushes.push_back(brush);
    return brush;
}

SHADER* MeshManager::createShader() {
    SHADER* shader = new SHADER;
    shader->brushes = new std::list<BRUSH*>;
    m_shaders.push_back(shader);
    return shader;
}

void MeshManager::addSurfaceToMesh(MESH* mesh, SURFACE* surface) {
    surface->pShader = mesh->pShader;
    mesh->surfaces->push_back(surface);
}

void MeshManager::addMeshToBrush(BRUSH* brush, MESH* mesh) {
    mesh->pShader = brush->pShader;
    brush->meshes->push_back(mesh);
}

void MeshManager::addBrushToShader(SHADER* shader, BRUSH* brush) {
    brush->pShader = shader;
    shader->brushes->push_back(brush);
}

void MeshManager::deleteSurface(SURFACE* surface) {

    // Durchlaufen aller Surfaces
    for (auto& mesh : m_meshes) {
        // Durchlaufen aller Surfaces im aktuellen Mesh
        auto& surfaces = *(mesh->surfaces);
        for (auto it = surfaces.begin(); it != surfaces.end(); ++it) {
            if (*it == surface) {
                // Surface gefunden, aus der Liste entfernen
                surfaces.erase(it);
                break; // da das Surface nur einmal in einem Mesh vorkomm
            }
        }
    }

    // Speicher freigeben
    auto it = std::find(m_surfaces.begin(), m_surfaces.end(), surface);
    if (it != m_surfaces.end()) {
        m_surfaces.erase(it);
        Memory::SafeDelete(surface);
    }
}

void MeshManager::deleteMesh(MESH* mesh) {
    // Durchlaufen aller Brushes
    for (auto& brush : m_brushes) {
        // Durchlaufen aller Meshes im aktuellen Brush
        auto& meshes = *(brush->meshes);
        for (auto it = meshes.begin(); it != meshes.end(); ++it) {
            if (*it == mesh) {
                // Mesh gefunden, aus der Liste entfernen
                meshes.erase(it);
                break;
            }
        }
    }

    // Speicher freigeben
    auto it = std::find(m_meshes.begin(), m_meshes.end(), mesh);
    if (it != m_meshes.end()) {
        m_meshes.erase(it);
        Memory::SafeDelete(mesh);
    }
}

void MeshManager::deleteBrush(BRUSH* brush) {
    // Durchlaufen aller Brushes
    for (auto& shader : m_shaders) {
        // Durchlaufen aller Brush im selben Shader
        auto& brushes = *(shader->brushes);
        for (auto it = brushes.begin(); it != brushes.end(); ++it) {
            if (*it == brush) {
                // Mesh gefunden, aus der Liste entfernen
                brushes.erase(it);
                break;
            }
        }
    }

    // Speicher freigeben
    auto it = std::find(m_brushes.begin(), m_brushes.end(), brush);
    if (it != m_brushes.end()) {
        m_brushes.erase(it);
        Memory::SafeDelete(brush);
    }
}

void MeshManager::removeSurfacefromMesh(MESH* mesh, SURFACE* surface) {
    // Durchlaufen aller Brushes

    auto& surfaces = *(mesh->surfaces);
    for (auto it = surfaces.begin(); it != surfaces.end(); ++it) {
        if (*it == surface) {
            // Surface gefunden, aus der Liste entfernen
            surfaces.erase(it);
            break;
        }
    }
}

void MeshManager::removeMeshfromBrush(BRUSH* brush, MESH* mesh) {
    // Durchlaufen aller Brushes

    auto& meshes = *(brush->meshes);
    for (auto it = meshes.begin(); it != meshes.end(); ++it) {
        if (*it == mesh) {
            // Mesh gefunden, aus der Liste entfernen
            meshes.erase(it);
            break;
        }
    }
}

void MeshManager::removeBrushfromShader(SHADER* shader, BRUSH* brush) {
    // Durchlaufen aller Brushes

    auto& brushes = *(shader->brushes);
    for (auto it = brushes.begin(); it != brushes.end(); ++it) {
        if (*it == brush) {
            // Brush gefunden, aus der Liste entfernen
            brushes.erase(it);
            break;
        }
    }
}

SURFACE* MeshManager::getPreviousSurface(SURFACE* currentSurface) {
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

MESH* MeshManager::getPreviousMesh(MESH* currentMesh) {
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

BRUSH* MeshManager::getPreviousBrush(BRUSH* currentBrush) {
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

// Funktion zum Zugriff auf das vorangegangene Element in m_shaders
SHADER* MeshManager::getPreviousShader(SHADER* currentShader)
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
    return nullptr; // Fall, wenn das übergebene Shader-Objekt nicht in der Liste gefunden wurde
}


SHADER* MeshManager::getShader(SURFACE surface)
{
    return surface.pShader;
}

SHADER* MeshManager::getShader(MESH mesh)
{
    return mesh.pShader;
}

SHADER* MeshManager::getShader(BRUSH brush)
{
    return brush.pShader;
}