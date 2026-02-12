#include "ObjectManager.h"

ObjectManager::ObjectManager() : m_device(nullptr) {
}

ObjectManager::~ObjectManager()
{
    // 1. Container clearen
    for (auto& shader : m_shaders) {
        shader->materials.clear();
    }
    for (auto& material : m_materials) {
        material->meshes.clear();
    }
    for (auto& mesh : m_meshes) {
        mesh->surfaces.clear();
    }

    // 2. Objekte löschen
    for (auto& surface : m_surfaces) {
        Memory::SafeDelete(surface);
    }
    m_surfaces.clear();

    for (auto& mesh : m_meshes) {
        Memory::SafeDelete(mesh);
    }
    m_meshes.clear();

    // ← NEU: Kameras löschen
    for (auto& camera : m_cameras) {
        Memory::SafeDelete(camera);
    }
    m_cameras.clear();

    for (auto& material : m_materials) {
        Memory::SafeDelete(material);
    }
    m_materials.clear();

    for (auto& shader : m_shaders) {
        Memory::SafeDelete(shader);
    }
    m_shaders.clear();

    // ← NEU: Entity-Liste clearen (enthält nur Referenzen)
    m_entities.clear();
}

void ObjectManager::Init(gdx::CDevice* device)
{
    m_device = device;
}

Surface* ObjectManager::createSurface() {
    Surface* surface = new Surface;
    m_surfaces.push_back(surface);
    return surface;
}

Mesh* ObjectManager::createMesh() {
    Mesh* mesh = new Mesh;
    m_meshes.push_back(mesh);
    m_entities.push_back(mesh);  // ← NEU: Zur Entity-Liste hinzufügen
    return mesh;
}

// ← NEU
Camera* ObjectManager::createCamera() {
    Camera* camera = new Camera;
    m_cameras.push_back(camera);
    m_entities.push_back(camera);  // ← Zur Entity-Liste hinzufügen
    return camera;
}

Material* ObjectManager::createMaterial() {
    Material* material = new Material;
    m_materials.push_back(material);
    return material;
}

Shader* ObjectManager::createShader() {
    Shader* shader = new Shader;
    m_shaders.push_back(shader);
    return shader;
}

//void ObjectManager::addSurfaceToMesh(Mesh* mesh, Surface* surface) {
//    surface->pShader = (Shader*)mesh->pShader;
//    mesh->AddSurfaceToMesh(surface);
//}

void ObjectManager::addSurfaceToMesh(Mesh* mesh, Surface* surface)
{
    if (!mesh || !surface) return;

    surface->pMesh = mesh; // wichtig
    mesh->AddSurfaceToMesh(surface);

    // optional (Übergang): surface->pShader weiter setzen, bis alles umgebaut ist
    // surface->pShader = mesh->pShader;
}

void ObjectManager::addMeshToMaterial(Material* material, Mesh* mesh) {
    mesh->pShader = (void*)material->pRenderShader;
    mesh->pMaterial = material;
    material->meshes.push_back(mesh);
}

void ObjectManager::addMaterialToShader(Shader* shader, Material* material)
{
    // Backwards compatibility wrapper for gidx.h
    assignShaderToMaterial(shader, material);
}

void ObjectManager::deleteSurface(Surface* surface) {
    for (auto& mesh : m_meshes) {
        auto& surfaces = mesh->surfaces;
        for (auto it = surfaces.begin(); it != surfaces.end(); ++it) {
            if (*it == surface) {
                surfaces.erase(it);
                break;
            }
        }
    }

    auto it = std::find(m_surfaces.begin(), m_surfaces.end(), surface);
    if (it != m_surfaces.end()) {
        m_surfaces.erase(it);
        Memory::SafeDelete(surface);
    }
}

void ObjectManager::deleteMesh(Mesh* mesh) {
    for (auto& material : m_materials) {
        auto& meshes = material->meshes;
        for (auto it = meshes.begin(); it != meshes.end(); ++it) {
            if (*it == mesh) {
                meshes.erase(it);
                break;
            }
        }
    }

    auto entIt = std::find(m_entities.begin(), m_entities.end(), mesh);
    if (entIt != m_entities.end()) {
        m_entities.erase(entIt);
    }

    auto it = std::find(m_meshes.begin(), m_meshes.end(), mesh);
    if (it != m_meshes.end()) {
        mesh->surfaces.clear();
        m_meshes.erase(it);
        Memory::SafeDelete(mesh);
    }
}

void ObjectManager::deleteCamera(Camera* camera) {
    // Aus Entity-Liste entfernen
    auto entIt = std::find(m_entities.begin(), m_entities.end(), camera);
    if (entIt != m_entities.end()) {
        m_entities.erase(entIt);
    }

    auto it = std::find(m_cameras.begin(), m_cameras.end(), camera);
    if (it != m_cameras.end()) {
        m_cameras.erase(it);
        Memory::SafeDelete(camera);
    }
}

void ObjectManager::deleteMaterial(Material* material)
{
    if (!material) return;

    // aus dem zugewiesenen Bucket raus (und sicherheitshalber Duplikate killen)
    Shader* sh = material->pRenderShader;
    if (sh)
    {
        auto& v = sh->materials;
        v.erase(std::remove(v.begin(), v.end(), material), v.end());
    }

    auto it = std::find(m_materials.begin(), m_materials.end(), material);
    if (it != m_materials.end())
    {
        material->meshes.clear();
        m_materials.erase(it);
        Memory::SafeDelete(material);
    }
}

void ObjectManager::removeSurfaceFromMesh(Mesh* mesh, Surface* surface) {
    auto& surfaces = mesh->surfaces;
    for (auto it = surfaces.begin(); it != surfaces.end(); ++it) {
        if (*it == surface) {
            surfaces.erase(it);
            break;
        }
    }
}

void ObjectManager::removeMeshFromMaterial(Material* material, Mesh* mesh) {
    auto& meshes = material->meshes;
    for (auto it = meshes.begin(); it != meshes.end(); ++it) {
        if (*it == mesh) {
            meshes.erase(it);
            break;
        }
    }
}

void ObjectManager::removeMaterialFromShader(Shader* shader, Material* material) {
    auto& materials = shader->materials;
    for (auto it = materials.begin(); it != materials.end(); ++it) {
        if (*it == material) {
            materials.erase(it);
            break;
        }
    }
}

Surface* ObjectManager::getPreviousSurface(Surface* currentSurface) {
    for (auto it = m_surfaces.begin(); it != m_surfaces.end(); ++it) {
        if (*it == currentSurface) {
            if (it != m_surfaces.begin()) {
                return *std::prev(it);
            }
            return nullptr;
        }
    }
    return nullptr;
}

Mesh* ObjectManager::getPreviousMesh(Mesh* currentMesh) {
    for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it) {
        if (*it == currentMesh) {
            if (it != m_meshes.begin()) {
                return *std::prev(it);
            }
            return nullptr;
        }
    }
    return nullptr;
}

Camera* ObjectManager::getPreviousCamera(Camera* currentCamera) {
    for (auto it = m_cameras.begin(); it != m_cameras.end(); ++it) {
        if (*it == currentCamera) {
            if (it != m_cameras.begin()) {
                return *std::prev(it);
            }
            return nullptr;
        }
    }
    return nullptr;
}

Material* ObjectManager::getPreviousMaterial(Material* currentMaterial) {
    for (auto it = m_materials.begin(); it != m_materials.end(); ++it) {
        if (*it == currentMaterial) {
            if (it != m_materials.begin()) {
                return *std::prev(it);
            }
            return nullptr;
        }
    }
    return nullptr;
}

Shader* ObjectManager::getPreviousShader(Shader* currentShader)
{
    for (auto it = m_shaders.begin(); it != m_shaders.end(); ++it) {
        if (*it == currentShader) {
            if (it != m_shaders.begin()) {
                return *std::prev(it);
            }
            return nullptr;
        }
    }
    return nullptr;
}

Surface* ObjectManager::getSurface(Mesh* mesh)
{
    if (!mesh->surfaces.empty()) {
        return mesh->surfaces.front();
    }
    return nullptr;
}

Material* ObjectManager::getStandardMaterial() const
{
    if (!m_materials.empty()) {
        return m_materials.front();
    }
    return nullptr;
}

void* ObjectManager::getShader(Surface surface) const
{
    return surface.pShader;
}

void* ObjectManager::getShader(Mesh mesh) const
{
    return mesh.pShader;
}

void* ObjectManager::getShader(Material material) const
{
    return material.pRenderShader;
}

void ObjectManager::processMesh()
{
    for (auto it = this->m_meshes.begin(); it != this->m_meshes.end(); ++it) {
        Mesh* mesh = *it;
        // Processing logic here
    }
}

void ObjectManager::deleteShader(Shader* shader)
{
    if (!shader) return;

    // Alle Materialien, die im Bucket hängen, vom Shader lösen
    for (auto* mat : shader->materials)
    {
        if (mat && mat->pRenderShader == shader)
            mat->pRenderShader = nullptr;
    }
    shader->materials.clear();

    auto it = std::find(m_shaders.begin(), m_shaders.end(), shader);
    if (it != m_shaders.end())
    {
        m_shaders.erase(it);
        Memory::SafeDelete(shader);
    }
}


void ObjectManager::assignShaderToMaterial(Shader* shader, Material* material)
{
    if (!material) return;

    // 1) Aus altem Bucket raus
    Shader* old = material->pRenderShader;
    if (old && old != shader)
    {
        auto& v = old->materials;
        v.erase(std::remove(v.begin(), v.end(), material), v.end()); // entfernt ALLE Duplikate
    }

    // 2) Wahrheit setzen
    material->pRenderShader = shader;

    // 3) In neuen Bucket rein (ohne Duplikate)
    if (shader)
    {
        auto& v = shader->materials;
        if (std::find(v.begin(), v.end(), material) == v.end())
            v.push_back(material);
    }

    // optional legacy sync (nur wenn du es noch brauchst)
    for (auto* mesh : material->meshes)
    {
        if (!mesh) continue;
        mesh->pShader = shader;
        for (auto* surface : mesh->surfaces)
            if (surface) surface->pShader = shader;
    }
}

