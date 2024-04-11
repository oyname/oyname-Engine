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

class RenderManager; // Vorwärtsdeklaration

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

    // Funktion zum Zugriff auf das vorangegangene Element in m_surfaces
    SURFACE* getPreviousSurface(SURFACE* currentSurface);

    // Funktion zum Zugriff auf das vorangegangene Element in m_meshes
    MESH* getPreviousMesh(MESH* currentMesh);

    // Funktion zum Zugriff auf das vorangegangene Element in m_brushes
    BRUSH* getPreviousBrush(BRUSH* currentBrush);

    // Funktion zum Zugriff auf das vorangegangene Element in m_shaders
    SHADER* getPreviousShader(SHADER* currentShader);

    SHADER* getShader(SURFACE surface);
    SHADER* getShader(MESH mesh);
    SHADER* getShader(BRUSH brush);

private:
    gdx::CDevice*       m_device;  
    std::list<Surface*> m_surfaces;
    std::list<Mesh*>    m_meshes;
    std::list<Brush*>   m_brushes;
    std::list<Shader*>  m_shaders;
};