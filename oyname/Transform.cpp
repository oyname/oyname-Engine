#include "Transform.h"

Transform::Transform() :
    position(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
    lookAt(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
    up(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),
    right(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)),
    rotation(DirectX::XMMatrixIdentity()),
    translation(DirectX::XMMatrixIdentity()),
    scaling(DirectX::XMMatrixIdentity())
{
    upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    forwardVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    rightVector = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
}

void Transform::Rotate(float fRotateX, float fRotateY, float fRotateZ, Space space) {
    DirectX::XMMATRIX rotationMatrix;

    DirectX::XMMATRIX rotationX = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(fRotateX));
    DirectX::XMMATRIX rotationY = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(fRotateY));
    DirectX::XMMATRIX rotationZ = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(fRotateZ));

    rotationMatrix = rotationZ * rotationX * rotationY;

    if (space == Space::Local) {
        rotation = rotationMatrix * rotation;
    }
    else if (space == Space::World) {
        rotation *= rotationMatrix;
    }

    lookAt = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(this->forwardVector, rotation));
    up = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(this->upVector, rotation));
    right = DirectX::XMVector3Cross(lookAt, up);
}

void Transform::Turn(float fRotateX, float fRotateY, float fRotateZ, Space space) {
    DirectX::XMMATRIX rotationX = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(fRotateX));
    DirectX::XMMATRIX rotationY = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(-fRotateY));
    DirectX::XMMATRIX rotationZ = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(fRotateZ));

    DirectX::XMMATRIX rotationMatrix = rotationZ * rotationX * rotationY;

    if (space == Space::Local) {
        rotation = rotationMatrix * rotation;
    }
    else if (space == Space::World) {
        rotation *= rotationMatrix;
    }

    lookAt = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(this->forwardVector, rotation));
    up = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(this->upVector, rotation));
    right = DirectX::XMVector3Cross(lookAt, up);
}

void Transform::Position(float x, float y, float z) {
    position = DirectX::XMVectorSet(x, y, z, 0.0f);
    translation = DirectX::XMMatrixTranslationFromVector(position);
}

void Transform::Move(float x, float y, float z, Space space) {
    DirectX::XMVECTOR trans = DirectX::XMVectorSet(x, y, z, 0.0f);
    if(space == Space::Local)
        trans = DirectX::XMVector3TransformCoord(trans, rotation);
    position = DirectX::XMVectorAdd(position, trans);
    translation = DirectX::XMMatrixTranslationFromVector(trans) * translation;
}

void Transform::Scale(float x, float y, float z) {
    scaling = DirectX::XMMatrixScaling(x, y, z);
}

void Transform::LookAt(const DirectX::XMVECTOR& target, const DirectX::XMVECTOR& upVector) {
    // Berechne die Blickrichtung 
    DirectX::XMVECTOR forward = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(target, position));

    // Y-Achse
    float angleY = atan2(DirectX::XMVectorGetX(forward), DirectX::XMVectorGetZ(forward));
    DirectX::XMMATRIX rotationY = DirectX::XMMatrixRotationY(angleY);

    // X-Achse
    float angleX = -asin(DirectX::XMVectorGetY(forward));
    DirectX::XMMATRIX rotationX = DirectX::XMMatrixRotationX(angleX);

    // Setze die Rotation
    rotation = rotationX * rotationY;

    // Aktualisieren
    lookAt = forward;
    right = DirectX::XMVector3Cross(upVector, forward);
    up = DirectX::XMVector3Cross(forward, right);
}

float Transform::Distance(const DirectX::XMVECTOR& target) 
{
    DirectX::XMVECTOR difference = DirectX::XMVectorSubtract(target, position);
    DirectX::XMVECTOR squaredDifference = DirectX::XMVector3LengthSq(difference);
    return sqrt(DirectX::XMVectorGetX(squaredDifference) + DirectX::XMVectorGetY(squaredDifference) + DirectX::XMVectorGetZ(squaredDifference));
}










