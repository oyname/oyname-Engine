#include "ObjectManager.h"
#include <algorithm>

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

    // Kameras löschen
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

    // Entity-Liste clearen (enthält nur Referenzen)
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
    m_entities.push_back(mesh);
    return mesh;
}

Camera* ObjectManager::createCamera() {
    Camera* camera = new Camera;
    m_cameras.push_back(camera);
    m_entities.push_back(camera);
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

void ObjectManager::addSurfaceToMesh(Mesh* mesh, Surface* surface)
{
    if (!mesh || !surface) return;

    surface->pMesh = mesh;
    mesh->AddSurfaceToMesh(surface);

    // optional (Übergang): surface->pShader weiter setzen, bis alles umgebaut ist
    // surface->pShader = mesh->pShader;
}

void ObjectManager::addMeshToMaterial(Material* material, Mesh* mesh) {
    if (!material || !mesh) return;

    // Keep the relationship consistent.
    mesh->pMaterial = material;

    if (std::find(material->meshes.begin(), material->meshes.end(), mesh) == material->meshes.end())
        material->meshes.push_back(mesh);

    // Ensure the material is part of the shader bucket used for rendering.
    // (Rendering walks: shader -> materials -> meshes -> surfaces)
    if (material->pRenderShader)
        assignShaderToMaterial(material->pRenderShader, material);
}

void ObjectManager::addMaterialToShader(Shader* shader, Material* material)
{
    assignShaderToMaterial(shader, material);
}

void ObjectManager::deleteSurface(Surface* surface) {
    if (!surface) return;

    // Fast path: surface knows its mesh.
    if (surface->pMesh)
    {
        auto& v = surface->pMesh->surfaces;
        v.erase(std::remove(v.begin(), v.end(), surface), v.end());
        surface->pMesh = nullptr;
    }
    else
    {
        // Fallback (legacy): search all meshes.
        for (auto& mesh : m_meshes) {
            auto& surfaces = mesh->surfaces;
            for (auto it = surfaces.begin(); it != surfaces.end(); ++it) {
                if (*it == surface) {
                    surfaces.erase(it);
                    break;
                }
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
    if (!mesh) return;

    // Detach from all materials
    for (auto& material : m_materials) {
        auto& meshes = material->meshes;
        for (auto it = meshes.begin(); it != meshes.end(); ++it) {
            if (*it == mesh) {
                meshes.erase(it);
                break;
            }
        }
    }

    // ObjectManager owns surfaces -> delete all surfaces of this mesh here.
    for (auto* s : mesh->surfaces)
    {
        if (!s) continue;
        s->pMesh = nullptr;

        auto sit = std::find(m_surfaces.begin(), m_surfaces.end(), s);
        if (sit != m_surfaces.end())
            m_surfaces.erase(sit);

        Memory::SafeDelete(s);
    }
    mesh->surfaces.clear();

    // Remove from entities
    auto entIt = std::find(m_entities.begin(), m_entities.end(), mesh);
    if (entIt != m_entities.end()) {
        m_entities.erase(entIt);
    }

    // Remove and delete mesh
    auto it = std::find(m_meshes.begin(), m_meshes.end(), mesh);
    if (it != m_meshes.end()) {
        m_meshes.erase(it);
        Memory::SafeDelete(mesh);
    }
}

void ObjectManager::deleteCamera(Camera* camera) {
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

    // remove from shader bucket
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

Shader* ObjectManager::getShader(const Surface& surface) const
{
    if (surface.pMesh && surface.pMesh->pMaterial)
        return surface.pMesh->pMaterial->pRenderShader;
    return nullptr;
}

Shader* ObjectManager::getShader(const Mesh& mesh) const
{
    if (mesh.pMaterial)
        return mesh.pMaterial->pRenderShader;
    return nullptr;
}

Shader* ObjectManager::getShader(const Material& material) const
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

    Shader* old = material->pRenderShader;
    if (old && old != shader)
    {
        auto& v = old->materials;
        v.erase(std::remove(v.begin(), v.end(), material), v.end());
    }

    material->pRenderShader = shader;

    if (shader)
    {
        auto& v = shader->materials;
        if (std::find(v.begin(), v.end(), material) == v.end())
            v.push_back(material);
    }
}

void ObjectManager::RegisterRenderable(Mesh* mesh)
{
    if (!mesh) return;
    m_renderMeshes.push_back(mesh);
}

void ObjectManager::UnregisterRenderable(Mesh* mesh)
{
    if (!mesh) return;
    m_renderMeshes.erase(
        std::remove(m_renderMeshes.begin(), m_renderMeshes.end(), mesh),
        m_renderMeshes.end()
    );
}


