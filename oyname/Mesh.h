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

class Mesh {
public:
    Mesh();
    ~Mesh();

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

    void TurnEntity(float fRotateX, float fRotateY, float fRotateZ, Space mode = Space::Local);
    void RotateEntity(float fRotateX, float fRotateY, float fRotateZ, Space mode = Space::Local);
    void PositionEntity(float x, float y, float z);
    void MoveEntity(float x, float y, float z);
    void Update();

    Shader* pShader;

private:
    DirectX::XMVECTOR defaultLookAt = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    DirectX::XMVECTOR defaultUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
};

typedef Mesh* LPMESH;
typedef Mesh MESH;


