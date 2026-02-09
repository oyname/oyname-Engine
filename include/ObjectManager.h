#pragma once
#include <vector>
#include "gdxutil.h"
#include "gdxdevice.h"
#include "gdxinterface.h"
#include "Entity.h"
#include "Surface.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"
#include "Shader.h"

using namespace DirectX;

class RenderManager;

class ObjectManager
{
    friend class RenderManager;
public:
    ObjectManager();
    ~ObjectManager();
    void Init(gdx::CDevice* device);

    // ==================== CREATE ====================
    Camera* createCamera();
    // Light* createLight();  // ← ENTFERNT: LightManager macht das
    Shader* createShader();
    Material* createMaterial();
    Mesh* createMesh();
    Surface* createSurface();

    // ==================== ADD ====================
    void addSurfaceToMesh(Mesh* mesh, Surface* surface);
    void addMeshToMaterial(Material* material, Mesh* mesh);
    void addMaterialToShader(Shader* shader, Material* material);

    // ==================== DELETE ====================
    void deleteSurface(Surface* surface);
    void deleteMesh(Mesh* mesh);
    void deleteCamera(Camera* camera);
    // void deleteLight(Light* light);  // ← ENTFERNT
    void deleteMaterial(Material* material);
    void deleteShader(Shader* shader);

    // ==================== REMOVE ====================
    void removeSurfaceFromMesh(Mesh* mesh, Surface* surface);
    void removeMeshFromMaterial(Material* material, Mesh* mesh);
    void removeMaterialFromShader(Shader* shader, Material* material);

    // ==================== GET PREVIOUS ====================
    Surface* getPreviousSurface(Surface* currentSurface);
    Mesh* getPreviousMesh(Mesh* currentMesh);
    Camera* getPreviousCamera(Camera* currentCamera);
    Material* getPreviousMaterial(Material* currentMaterial);
    Shader* getPreviousShader(Shader* currentShader);

    // ==================== GET ====================
    void processMesh();
    Surface* getSurface(Mesh* mesh);
    Material* getStandardMaterial() const;
    void* getShader(Surface surface) const;
    void* getShader(Mesh mesh) const;
    void* getShader(Material material) const;

private:
    gdx::CDevice* m_device;
    std::vector<Entity*> m_entities;     // Alle Entities (Mesh + Camera)
    std::vector<Surface*> m_surfaces;
    std::vector<Mesh*> m_meshes;
    std::vector<Camera*> m_cameras;
    // std::vector<Light*> m_lights;     // ← ENTFERNT: In LightManager
    std::vector<Material*> m_materials;
    std::vector<Shader*> m_shaders;
};