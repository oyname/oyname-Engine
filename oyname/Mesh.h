#pragma once
#include <list>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Surface.h"   
#include "gdxutil.h"

class Shader;

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

    ConstantBuffer cb;

    std::list<Surface*>* surfaces;

    ID3D11Buffer* constantBuffer;

    void TurnEntity(float fRotateX, float fRotateY, float fRotateZ);
    void RotateEntity(float fRotateX, float fRotateY, float fRotateZ);
    void PositionEntity(float x, float y, float z);
    void MoveEntity(float x, float y, float z);
    void Update();

    DirectX::XMMATRIX projectionMatrix() { return cb.projectionMatrix; }
    DirectX::XMMATRIX viewMatrix() { return cb.viewMatrix; }
    DirectX::XMMATRIX worldMatrix() { return cb.worldMatrix; }

    Shader* pShader;
};

typedef Mesh* LPMESH;
typedef Mesh MESH;


