#include "Object3D.h"

using namespace DirectX;

Object3D::Object3D() {}

void Object3D::Init(gdx::CGIDX* engine)
{
    m_engine = engine;
}

void Object3D::PositionEntity(LPMESH mesh, float x, float y, float z) {
    mesh->position = XMVectorSet(x, y, z, 0.0f);
    XMMATRIX mTranslation = XMMatrixTranslationFromVector(mesh->position);
    mesh->mTranslation = mTranslation;
}

void Object3D::MoveEntity(LPMESH mesh, float x, float y, float z) {
    XMVECTOR translation = XMVectorSet(x, y, z, 0.0f);
    translation = XMVector3TransformCoord(translation, mesh->mRotate);
    mesh->position = XMVectorAdd(mesh->position, translation);
    XMMATRIX mTranslation = XMMatrixTranslationFromVector(translation);
    mesh->mTranslation *= mTranslation;
}

void Object3D::RotateEntity(LPMESH lpMesh, float fRotateX, float fRotateY, float fRotateZ) {
    fRotateX = XMConvertToRadians(fRotateX);
    fRotateY = XMConvertToRadians(fRotateY);
    fRotateZ = XMConvertToRadians(fRotateZ);

    XMMATRIX rotationX = XMMatrixRotationX(fRotateX);
    XMMATRIX rotationY = XMMatrixRotationY(fRotateY);
    XMMATRIX rotationZ = XMMatrixRotationZ(fRotateZ);
    XMMATRIX rotationMatrix = rotationX * rotationY * rotationZ;

    XMMATRIX newRotationMatrix = rotationMatrix;
    lpMesh->mRotate = rotationMatrix;

    lpMesh->lookAt = XMVector3TransformCoord(lpMesh->lookAt, newRotationMatrix);

    XMFLOAT3 upVector = { 0.0f, 1.0f, 0.0f };
    XMVECTOR up = XMLoadFloat3(&upVector);
    lpMesh->up = XMVector3TransformCoord(up, newRotationMatrix);
}

void Object3D::TurnEntity(LPMESH lpMesh, float fRotateX, float fRotateY, float fRotateZ) {
    XMMATRIX rotationX = XMMatrixRotationX(XMConvertToRadians(fRotateX));
    XMMATRIX rotationY = XMMatrixRotationY(XMConvertToRadians(fRotateY));
    XMMATRIX rotationZ = XMMatrixRotationZ(XMConvertToRadians(fRotateZ));
    XMMATRIX rotationMatrix = rotationX * rotationY * rotationZ;
    XMMATRIX newRotationMatrix = rotationMatrix * lpMesh->mRotate;

    lpMesh->mRotate = newRotationMatrix;

    lpMesh->lookAt = XMVector3TransformCoord(lpMesh->lookAt, newRotationMatrix);

    XMFLOAT3 upVector = { 0.0f, 1.0f, 0.0f };
    XMVECTOR up = XMLoadFloat3(&upVector);
    lpMesh->up = XMVector3TransformCoord(up, newRotationMatrix);
}