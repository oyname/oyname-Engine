#pragma once
#include <list>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Surface.h"   
#include "gdxutil.h"

class Shader;

enum Space 
{
    Local,
    World
};

struct MatrixSet
{
    DirectX::XMMATRIX viewMatrix;
    DirectX::XMMATRIX projectionMatrix;
    DirectX::XMMATRIX worldMatrix;
    DirectX::XMFLOAT3 lightDirection;
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

private:
    DirectX::XMVECTOR defaultLookAt = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    DirectX::XMVECTOR defaultUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
};

typedef Mesh* LPMESH;
typedef Mesh* LPCAMERA;
typedef Mesh MESH;
typedef Mesh CAMERA;



