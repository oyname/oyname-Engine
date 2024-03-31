#pragma once

#include <list>
#include "gdxutil.h"
#include "gdxdevice.h"
#include "gdxinterface.h"

using namespace DirectX;

// Forward declaration of SHADER
struct SHADER;
typedef SHADER* LPSHADER; // Typedef for pointer to SHADER

struct SURFACE
{
    bool isActive;                      // Gibt an, ob die Oberfl�che aktiv ist oder nicht

    // Vertices, Color, ....
    std::vector<DirectX::XMFLOAT3> position;  
    unsigned int size_vertex;           // Gr��e der Vertices
    unsigned int size_listVertices;     // Gr��e der Liste (nicht spezifiziert)

    std::vector<DirectX::XMFLOAT4> color;
    unsigned int size_color;            // Gr��e der Vertices
    unsigned int size_listColor;        // Gr��e der Liste (nicht spezifiziert)

    // Indices
    std::vector<unsigned int> indices;
    unsigned int size_listIndex;

    ID3D11Buffer* positionBuffer;       // Buffer f�r die Oberfl�che
    ID3D11Buffer* colorBuffer;          // Buffer f�r die Oberfl�che
    ID3D11Buffer* indexBuffer;          // Buffer f�r die Oberfl�che
    SHADER* pShader;
    
    // Standardkonstruktor
    SURFACE() :
        isActive(false),
        size_vertex(0),
        size_listVertices(0),
        size_color(0),
        size_listColor(0),
        size_listIndex(0),
        positionBuffer(nullptr),
        colorBuffer(nullptr),
        indexBuffer(nullptr),
        pShader(nullptr)
    {

    }

    ~SURFACE() 
    {
        Memory::SafeRelease(positionBuffer);
        Memory::SafeRelease(colorBuffer);
        Memory::SafeRelease(indexBuffer);
    }
} typedef* LPSURFACE;

struct MESH
{    
    bool isActive;

    XMVECTOR position;     // Position des Objekts
    XMVECTOR lookAt;       // Blickrichtung
    XMVECTOR up;           // Up-Vektor

    ConstantBuffer cb;

    std::list<SURFACE*>* surfaces;

    ID3D11Buffer* constantBuffer;

    SHADER* pShader;

    // Standardkonstruktor
    MESH() :
        constantBuffer(nullptr),
        isActive(false),
        surfaces(nullptr),
        pShader(nullptr),
        position(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
        lookAt(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
        up(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
    {
        cb.projectionMatrix = DirectX::XMMatrixIdentity();
        cb.viewMatrix = DirectX::XMMatrixIdentity();
        cb.worldMatrix = DirectX::XMMatrixIdentity();
    }

}typedef *LPMESH;

struct BRUSH 
{
    bool isActive;

    float shininess;
    float transparency;

    DirectX::XMFLOAT4 diffuseColor;
    DirectX::XMFLOAT4 specularColor;

    ID3D11ShaderResourceView* pTexture;

    std::list<MESH*>* meshes;
    SHADER* pShader;

    // Standardkonstruktor
    BRUSH() :
        isActive(false),
        shininess(0.0f),
        transparency(0.0f),
        diffuseColor(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)),
        specularColor(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)),
        pTexture(nullptr),
        meshes(nullptr),
        pShader(nullptr)
    {
        // Hier k�nnen weitere Initialisierungen durchgef�hrt werden
    }
}typedef *LPBRUSH;

struct SHADER
{
    bool isActive;                      // Gibt an, ob der Shader aktiv ist oder nicht

    DWORD flags;                        // Flags zur Konfiguration des Vertex (nicht spezifiziert)
    void* bytecodeVS;                   // Zeiger auf den Bytecode des Vertex-Shaders
    unsigned int sizeVS;                // Gr��e des Vertex-Shaders
    std::wstring vertexShaderFile;      // Dateiname des Vertex-Shaders
    std::wstring pixelShaderFile;       // Dateiname des Pixel-Shaders

    ID3D11InputLayout* inputlayout;     // Input-Layout f�r den Shader
    ID3D11VertexShader* vertexShader;   // Zeiger auf den Vertex-Shader
    ID3D11PixelShader* pixelShader;     // Zeiger auf den Pixel-Shader
    ID3D10Blob* blobVS;                 // Blob f�r den Vertex-Shader
    ID3D10Blob* blobPS;                 // Blob f�r den Pixel-Shader

    std::list<BRUSH*>* brushes;         // Zeiger auf eine Liste von Zeigern auf Brushes

    // Standardkonstruktor
    SHADER() :
        isActive(false),
        flags(0),
        bytecodeVS(nullptr),
        sizeVS(0),
        inputlayout(nullptr),
        vertexShader(nullptr),
        pixelShader(nullptr),
        blobVS(nullptr),
        blobPS(nullptr),
        brushes(nullptr)
    {
        // Hier k�nnen weitere Initialisierungen durchgef�hrt werden
    }

    ~SHADER() 
    {
        Memory::SafeRelease(inputlayout);
        Memory::SafeRelease(vertexShader);
        Memory::SafeRelease(pixelShader);
        Memory::SafeRelease(blobVS);
        Memory::SafeRelease(blobPS);
    }
} typedef* LPSHADER;

class RenderManager; // Vorw�rtsdeklaration

class ObjectManager {

    friend class RenderManager; 

public:
    ObjectManager();
    ~ObjectManager();

    void Init(gdx::CDevice& device);

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


    void RenderLoop(); // leere Methode siehe RenderManager

private:
    gdx::CDevice*       m_device;      
    std::list<SURFACE*> m_surfaces;
    std::list<MESH*>    m_meshes;
    std::list<BRUSH*>   m_brushes;
    std::list<SHADER*>  m_shaders;
};