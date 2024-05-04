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
    ENTITY* createMesh();
    BRUSH* createBrush();
    SHADER* createShader();

    void addSurfaceToMesh(ENTITY* mesh, SURFACE* surface);
    void addMeshToBrush(BRUSH* brush, ENTITY* mesh);
    void addBrushToShader(SHADER* shader, BRUSH* brush);

    void deleteSurface(SURFACE* surface);
    void deleteMesh(ENTITY* mesh);
    void deleteBrush(BRUSH* brush);

    void removeSurfacefromMesh(ENTITY* mesh, SURFACE* surface);
    void removeMeshfromBrush(BRUSH* brush, ENTITY* mesh);
    void removeBrushfromShader(SHADER* shader, BRUSH* brush);

    SURFACE* getPreviousSurface(SURFACE* currentSurface);
    ENTITY* getPreviousMesh(ENTITY* currentMesh);
    BRUSH* getPreviousBrush(BRUSH* currentBrush);
    SHADER* getPreviousShader(SHADER* currentShader);


    void processMesh();

    SURFACE* getSurface(ENTITY* mesh);

    // Standardbrush
    BRUSH* getStandardBrush() const;

    void* getShader(SURFACE surface) const;
    void* getShader(ENTITY mesh) const;
    void* getShader(BRUSH brush) const;

private:
    gdx::CDevice*       m_device;  
    std::list<Surface*> m_surfaces;
    std::list<Mesh*>    m_meshes;
    std::list<Brush*>   m_brushes;
    std::list<Shader*>  m_shaders;
};