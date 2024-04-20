#pragma once

#include <list>
#include "gdxutil.h"
#include "gdxdevice.h"
#include "gdxinterface.h"

#include "Surface.h"
#include "Mesh.h"
#include "Brush.h"
#include "Shader.h"
#include "Light.h"

using namespace DirectX;

// Forward declaration
class RenderManager;

class ObjectManager 
{
    friend class RenderManager; 

public:
    ObjectManager();
    ~ObjectManager();

    void Init(gdx::CDevice* device);

    SURFACE* createSurface(); 
    MESH* createMesh();
    BRUSH* createBrush();
    SHADER* createShader();

    void addSurfaceToMesh(MESH* mesh, SURFACE* surface);
    void addMeshToBrush(BRUSH* brush, MESH* mesh);
    void addBrushToShader(SHADER* shader, BRUSH* brush);

    void deleteSurface(SURFACE* surface);
    void deleteMesh(MESH* mesh);
    void deleteBrush(BRUSH* brush);

    void removeSurfacefromMesh(MESH* mesh, SURFACE* surface);
    void removeMeshfromBrush(BRUSH* brush, MESH* mesh);
    void removeBrushfromShader(SHADER* shader, BRUSH* brush);

    SURFACE* getPreviousSurface(SURFACE* currentSurface);
    MESH* getPreviousMesh(MESH* currentMesh);
    BRUSH* getPreviousBrush(BRUSH* currentBrush);
    SHADER* getPreviousShader(SHADER* currentShader);

    SURFACE* getSurface(MESH* mesh);

    // Standardbrush
    BRUSH* getStandardBrush() const;

    void* getShader(SURFACE surface) const;
    void* getShader(MESH mesh) const;
    void* getShader(BRUSH brush) const;

private:
    gdx::CDevice*       m_device;  
    std::list<Surface*> m_surfaces;
    std::list<Mesh*>    m_meshes;
    std::list<Brush*>   m_brushes;
    std::list<Shader*>  m_shaders;
};