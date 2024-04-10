#pragma once
#include <list>
#include <d3d11.h>
#include <DirectXMath.h>
#include "gdxutil.h"
#include "Surface.h"   

class Shader;

enum Space 
{
    Local,
    World
};

__declspec(align(16))
struct MatrixSet
{
    DirectX::XMMATRIX viewMatrix;
    DirectX::XMMATRIX projectionMatrix;
    DirectX::XMMATRIX worldMatrix;
};

class Mesh 
{
public:
    Mesh();
    ~Mesh();

    void TurnEntity(float fRotateX, float fRotateY, float fRotateZ, Space mode = Space::Local);
    void RotateEntity(float fRotateX, float fRotateY, float fRotateZ, Space mode = Space::Local);
    void PositionEntity(float x, float y, float z);
    void MoveEntity(float x, float y, float z);
    void ScaleEntity(float x, float y, float z);
    
    void UpdateConstantBuffer(const gdx::CDevice* device, const DirectX::XMMATRIX view, const DirectX::XMMATRIX proj);
    void Update();

    bool isActive;

    DirectX::XMVECTOR position;
    DirectX::XMVECTOR lookAt;
    DirectX::XMVECTOR up;
    DirectX::XMVECTOR right;

    DirectX::XMMATRIX mRotate;
    DirectX::XMMATRIX mTranslation;
    DirectX::XMMATRIX mScaling;

    MatrixSet cb;

    std::list<Surface*>* surfaces;

    ID3D11Buffer* constantBuffer;

    Shader* pShader;

    void* operator new(size_t size) {
        // Ausrichtung auf 16 Bytes 
        return _aligned_malloc(size, 16);
    }

    void operator delete(void* p) noexcept {
        // richtigen Speicherdeallokator
        _aligned_free(p);
    }

private:
    DirectX::XMVECTOR defaultLookAt = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    DirectX::XMVECTOR defaultUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
};

typedef Mesh* LPMESH;
typedef Mesh* LPCAMERA;
typedef Mesh MESH;
typedef Mesh CAMERA;



