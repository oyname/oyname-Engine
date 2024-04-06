#include "Mesh.h"

Mesh::Mesh() :
    isActive(false),
    constantBuffer(nullptr),
    surfaces(nullptr),
    position(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
    lookAt(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
    up(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),
    right(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)),
    mRotate(DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(0.0f)))
{
    cb.projectionMatrix = DirectX::XMMatrixIdentity();
    cb.viewMatrix = DirectX::XMMatrixIdentity();
    cb.worldMatrix = DirectX::XMMatrixIdentity();
}

Mesh::~Mesh() {
    Memory::SafeRelease(constantBuffer);
}

void Mesh::TurnEntity(float fRotateX, float fRotateY, float fRotateZ) {
    DirectX::XMMATRIX rotationX = DirectX::XMMatrixRotationAxis(right, DirectX::XMConvertToRadians(fRotateX));
    DirectX::XMMATRIX rotationY = DirectX::XMMatrixRotationAxis(up, DirectX::XMConvertToRadians(fRotateY));
    DirectX::XMMATRIX rotationZ = DirectX::XMMatrixRotationAxis(lookAt, DirectX::XMConvertToRadians(fRotateZ));

    DirectX::XMMATRIX rotationMatrix = rotationX * rotationY * rotationZ;

    DirectX::XMMATRIX newRotationMatrix = rotationMatrix * mRotate;

    mRotate = newRotationMatrix;

    lookAt = DirectX::XMVector3TransformCoord(lookAt, rotationMatrix);
    up = DirectX::XMVector3TransformCoord(up, rotationMatrix);
}

void Mesh::RotateEntity(float fRotateX, float fRotateY, float fRotateZ) {
    float roll = DirectX::XMConvertToRadians(fRotateX);
    float pitch = DirectX::XMConvertToRadians(fRotateY);
    float yaw = DirectX::XMConvertToRadians(fRotateZ);
    mRotate = DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw);

    lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    right = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

    lookAt = DirectX::XMVector3TransformCoord(lookAt, mRotate);
    up = DirectX::XMVector3TransformCoord(up, mRotate);
}

void Mesh::PositionEntity(float x, float y, float z) {
    position = DirectX::XMVectorSet(x, y, z, 0.0f);
    mTranslation = DirectX::XMMatrixTranslationFromVector(position);
}

void Mesh::MoveEntity(float x, float y, float z) {
    DirectX::XMVECTOR translation = DirectX::XMVectorSet(x, y, z, 0.0f);
    translation = DirectX::XMVector3TransformCoord(translation, mRotate);
    position = DirectX::XMVectorAdd(position, translation);
    mTranslation = DirectX::XMMatrixTranslationFromVector(translation) * mTranslation;
}

void Mesh::Update() {
    cb.viewMatrix = DirectX::XMMatrixLookToLH(position, lookAt, up);
}
