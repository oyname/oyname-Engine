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
    bool isActive;                      // Gibt an, ob die Oberfläche aktiv ist oder nicht

    // Vertices, Color, ....
    std::vector<DirectX::XMFLOAT3> position;  
    unsigned int size_vertex;           // Größe der Vertices
    unsigned int size_listVertices;     // Größe der Liste (nicht spezifiziert)

    std::vector<DirectX::XMFLOAT4> color;
    unsigned int size_color;            // Größe der Vertices
    unsigned int size_listColor;        // Größe der Liste (nicht spezifiziert)

    // Indices
    std::vector<unsigned int> indices;
    unsigned int size_listIndex;

    ID3D11Buffer* positionBuffer;       // Buffer für die Oberfläche
    ID3D11Buffer* colorBuffer;          // Buffer für die Oberfläche
    ID3D11Buffer* indexBuffer;          // Buffer für die Oberfläche
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
    XMVECTOR right;        // Right-Vektor

    XMMATRIX mRotate;
    XMMATRIX mTranslation;
    XMMATRIX mScaling;

    XMFLOAT3 vRight;
    XMFLOAT3 vUp;
    XMFLOAT3 vLook;

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
        vRight(XMFLOAT3(1.0f,0.0f,0.0f)),
        vUp(XMFLOAT3(0.0f, 1.0f, 0.0f)),
        vLook(XMFLOAT3(0.0f, 0.0f, 1.0f)),
        position(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
        lookAt(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
        up(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),
        right(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)),
        mRotate(XMMatrixRotationY(XMConvertToRadians(0.0f)))
    {
        cb.projectionMatrix = DirectX::XMMatrixIdentity();
        cb.viewMatrix = DirectX::XMMatrixIdentity();
        cb.worldMatrix = DirectX::XMMatrixIdentity();
    }

    void TurnEntityEx(float fRotateX, float fRotateY, float fRotateZ, bool obj = false)
    {
        // Konstruieren Sie die Rotationsmatrizen um die lokalen Achsen
        XMMATRIX rotationX = XMMatrixRotationX(XMConvertToRadians(fRotateX));
        XMMATRIX rotationY = XMMatrixRotationY(XMConvertToRadians(fRotateY));
        XMMATRIX rotationZ = XMMatrixRotationZ(XMConvertToRadians(fRotateZ));

        // Kombinieren Sie die Rotationsmatrizen in eine Gesamtrotationsmatrix
        XMMATRIX rotationMatrix = rotationX * rotationY * rotationZ;

        // Wenden Sie die Rotation auf die lokale Rotationsmatrix des Objekts an
        XMMATRIX newRotationMatrix = rotationMatrix * mRotate;

        mRotate = newRotationMatrix;
    }

    // Funktion zur Rotation eines Objekts um seine lokalen Achsen
    void TurnEntity(float fRotateX, float fRotateY, float fRotateZ, bool obj = false)
    {
        //lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        //up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        //right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

        XMMATRIX rotationX = XMMatrixRotationAxis(right, XMConvertToRadians(fRotateX));
        XMMATRIX rotationY = XMMatrixRotationAxis(up, XMConvertToRadians(fRotateY));
        XMMATRIX rotationZ = XMMatrixRotationAxis(lookAt, XMConvertToRadians(fRotateZ));

        // Kombinieren der Rotationsmatrizen in eine Gesamtrotationsmatrix
        XMMATRIX rotationMatrix = rotationX * rotationY * rotationZ;

        // Wenden Sie die Rotation auf die lokale Rotationsmatrix des Objekts an
        XMMATRIX newRotationMatrix = rotationMatrix * mRotate;

        mRotate = newRotationMatrix;

        // Anwenden der Rotation auf die lokalen Achsen (lookAt und up)
        lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
        up = XMVector3TransformCoord(up, rotationMatrix);

        if (!obj)
            // Aktualisieren der View-Matrix
            cb.viewMatrix = XMMatrixLookToLH(position, lookAt, up);
    }

    void RotateEntity(float fRotateX, float fRotateY, float fRotateZ, bool obj = false)
    {
        float roll  = XMConvertToRadians(fRotateX);
        float pitch = XMConvertToRadians(fRotateY);;
        float yaw   = XMConvertToRadians(fRotateZ);
        mRotate = XMMatrixRotationRollPitchYaw(roll, pitch, yaw);

        lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

        // Anwenden der Rotation auf die lokalen Achsen (lookAt und up)
        lookAt = XMVector3TransformCoord(lookAt, mRotate);
        up = XMVector3TransformCoord(up, mRotate);

        if(!obj)
            // Aktualisieren der View-Matrix
            cb.viewMatrix = XMMatrixLookToLH(position, lookAt, up);
    }

    void Rotate(float fRotateX, float fRotateY, float fRotateZ)
    {
        // Konvertieren der Rotationswinkel von Grad in Radiant
        fRotateX = XMConvertToRadians(fRotateX);
        fRotateY = XMConvertToRadians(fRotateY);
        fRotateZ = XMConvertToRadians(fRotateZ);

        // Erstellen der Rotationsmatrizen für jede Achse
        XMMATRIX rotationX = XMMatrixRotationX(fRotateX);
        XMMATRIX rotationY = XMMatrixRotationY(fRotateY);
        XMMATRIX rotationZ = XMMatrixRotationZ(fRotateZ);

        // Kombinieren der Rotationsmatrizen
        mRotate = rotationX * rotationY * rotationZ;

        // Transformieren Sie die Kamerarichtung und die Up-Vektoren entsprechend
        lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // Setzen Sie den lookAt-Vektor auf die Standard-Blickrichtung
        up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // Setzen Sie den Up-Vektor auf die Standard-Up-Richtung
        lookAt = XMVector3TransformCoord(lookAt, mRotate);
        up = XMVector3TransformCoord(up, mRotate);

        // Aktualisieren Sie die View-Matrix
        cb.viewMatrix = XMMatrixLookToLH(position, lookAt, up);
    }

    void PositionEntity(float x, float y, float z)
    {
        // Aktualisieren Sie die Position des Meshes um den Translationsvektor
        position = XMVectorSet(x, y, z, 1.0f); // W-Wert auf 1 setzen

        // Aktualisieren der View-Matrix
        cb.viewMatrix = XMMatrixLookToLH(position, lookAt, up);
    }

    XMMATRIX projectionMatrix() { return cb.projectionMatrix; }
    XMMATRIX viewMatrix() { return cb.viewMatrix; }
    XMMATRIX wordlMaterix() { return cb.worldMatrix; }

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
        // Hier können weitere Initialisierungen durchgeführt werden
    }
}typedef *LPBRUSH;

struct SHADER
{
    bool isActive;                      // Gibt an, ob der Shader aktiv ist oder nicht

    DWORD flags;                        // Flags zur Konfiguration des Vertex (nicht spezifiziert)
    void* bytecodeVS;                   // Zeiger auf den Bytecode des Vertex-Shaders
    unsigned int sizeVS;                // Größe des Vertex-Shaders
    std::wstring vertexShaderFile;      // Dateiname des Vertex-Shaders
    std::wstring pixelShaderFile;       // Dateiname des Pixel-Shaders

    ID3D11InputLayout* inputlayout;     // Input-Layout für den Shader
    ID3D11VertexShader* vertexShader;   // Zeiger auf den Vertex-Shader
    ID3D11PixelShader* pixelShader;     // Zeiger auf den Pixel-Shader
    ID3D10Blob* blobVS;                 // Blob für den Vertex-Shader
    ID3D10Blob* blobPS;                 // Blob für den Pixel-Shader

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
        // Hier können weitere Initialisierungen durchgeführt werden
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

class RenderManager; // Vorwärtsdeklaration

class MeshManager {

    friend class RenderManager; 

public:
    MeshManager();
    ~MeshManager();

    void Init(gdx::CDevice* device, gdx::CGIDX* engine);

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
    gdx::CGIDX*         m_engine;
    gdx::CDevice*       m_device;  
    std::list<SURFACE*> m_surfaces;
    std::list<MESH*>    m_meshes;
    std::list<BRUSH*>   m_brushes;
    std::list<SHADER*>  m_shaders;
};