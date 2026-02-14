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

    // CREATE
    Camera* createCamera();
    Shader* createShader();
    Material* createMaterial();
    Mesh* createMesh();
    Surface* createSurface();

    void RegisterRenderable(Mesh* mesh);
    void UnregisterRenderable(Mesh* mesh);

    // ADD
    void addSurfaceToMesh(Mesh* mesh, Surface* surface);
    void addMeshToMaterial(Material* material, Mesh* mesh);

    // Assign shader to material and keep buckets in sync
    void assignShaderToMaterial(Shader* shader, Material* material);

    // Backwards compatibility (old name used by gidx.h)
    void addMaterialToShader(Shader* shader, Material* material);

    // DELETE
    void deleteSurface(Surface* surface);
    void deleteMesh(Mesh* mesh);
    void deleteCamera(Camera* camera);
    void deleteMaterial(Material* material);
    void deleteShader(Shader* shader);

    // REMOVE
    void removeSurfaceFromMesh(Mesh* mesh, Surface* surface);
    void removeMeshFromMaterial(Material* material, Mesh* mesh);
    void removeMaterialFromShader(Shader* shader, Material* material);

    // GET PREVIOUS
    Surface* getPreviousSurface(Surface* currentSurface);
    Mesh* getPreviousMesh(Mesh* currentMesh);
    Camera* getPreviousCamera(Camera* currentCamera);
    Material* getPreviousMaterial(Material* currentMaterial);
    Shader* getPreviousShader(Shader* currentShader);

    // GET
    void processMesh();
    Surface* getSurface(Mesh* mesh);
    Material* getStandardMaterial() const;
    Shader* getShader(const Surface& surface) const;
    Shader* getShader(const Mesh& mesh) const;
    Shader* getShader(const Material& material) const;

private:
    gdx::CDevice* m_device;
    std::vector<Entity*> m_entities;
    std::vector<Surface*> m_surfaces;
    std::vector<Mesh*> m_meshes;
    std::vector<Mesh*> m_renderMeshes;
    std::vector<Camera*> m_cameras;
    std::vector<Material*> m_materials;
    std::vector<Shader*> m_shaders;
};

