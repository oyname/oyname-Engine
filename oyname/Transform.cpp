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

}

void Transform::Rotate(float fRotateX, float fRotateY, float fRotateZ, Space space) 
{
    DirectX::XMMATRIX rotationX = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(fRotateX));
    DirectX::XMMATRIX rotationY = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(fRotateY));
    DirectX::XMMATRIX rotationZ = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(fRotateZ));
    
    DirectX::XMMATRIX rotationMatrix = rotationZ * rotationX * rotationY;
    
    if (space == Space::World) {
        rotationMatrix *= rotation;
        rotation = *this->worldMatrix * rotationMatrix;
    }
    else if (space == Space::Local) {
        rotation *= rotationMatrix;
    }
    
    // Extrahiere die Vektoren aus der Rotationsmatrix
    lookAt = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotation));
    up = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotation));
    right = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotation));
}

void Transform::Turn(float fRotateX, float fRotateY, float fRotateZ, Space space) 
{
    DirectX::XMMATRIX rotationX = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(fRotateX));
    DirectX::XMMATRIX rotationY = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(-fRotateY));
    DirectX::XMMATRIX rotationZ = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(fRotateZ));
    
    DirectX::XMMATRIX rotationMatrix = rotationZ * rotationX * rotationY;
  
 
    if (space == Space::World) {
        rotationMatrix *= rotation;
        rotation = *this->worldMatrix * rotationMatrix;
    }
    else if (space == Space::Local) {
        rotation *= rotationMatrix;
    }
    
    // Extrahiere die Vektoren aus der Rotationsmatrix
    lookAt = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotation));
    up = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotation));
    right = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotation));
}

void Transform::Position(float x, float y, float z) 
{
    position = DirectX::XMVectorSet(x, y, z, 0.0f);
    translation = DirectX::XMMatrixTranslationFromVector(position);
}

void Transform::Move(float x, float y, float z, Space space) 
{
    DirectX::XMVECTOR trans = DirectX::XMVectorSet(x, y, z, 0.0f);
    if (space == Space::Local)
        trans = DirectX::XMVector3TransformCoord(trans, rotation);
    position = DirectX::XMVectorAdd(position, trans);
    translation = DirectX::XMMatrixTranslationFromVector(trans) * translation;
}

void Transform::Scale(float x, float y, float z) 
{
    scaling = DirectX::XMMatrixScaling(x, y, z);
}

void Transform::LookAt(const DirectX::XMVECTOR& target, const DirectX::XMVECTOR& upVector) 
{
    // Berechne die Blickrichtung 
    DirectX::XMVECTOR forward = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(target, position));
    
    // Z-Achse
    float angleZ = this->ExtractRoll(&rotation);
    DirectX::XMMATRIX rotationZ = DirectX::XMMatrixRotationZ(angleZ);
    
    // Y-Achse
    float angleY = atan2(DirectX::XMVectorGetX(forward), DirectX::XMVectorGetZ(forward));
    DirectX::XMMATRIX rotationY = DirectX::XMMatrixRotationY(angleY);
    
    // X-Achse
    float angleX = -asin(DirectX::XMVectorGetY(forward));
    DirectX::XMMATRIX rotationX = DirectX::XMMatrixRotationX(angleX);
    
    // Setze die Rotation
    rotation = rotationZ * rotationX * rotationY;
    
    lookAt = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotation));
    up = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(upVector, rotation));
    right = DirectX::XMVector3Cross(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void Transform::RotateTowardsVector(const DirectX::XMVECTOR& targetDirection, const DirectX::XMVECTOR& upVector)
{
    // Normalisiere den Zielvektor
    DirectX::XMVECTOR forward = DirectX::XMVector3Normalize(targetDirection);

    // Berechne die lokale Z-Achse (lookAt)
    lookAt = forward;

    // Berechne die lokale X-Achse (right) als Kreuzprodukt zwischen der "upVector" und der neuen Blickrichtung
    right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(upVector, forward));

    // Berechne die lokale Y-Achse (up) als Kreuzprodukt zwischen der neuen Blickrichtung und der neuen rechten Achse
    up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(forward, right));

    // Berechne die Rotation basierend auf den lokalen Achsen
    DirectX::XMMATRIX rotationMatrix(
        right.m128_f32[0], right.m128_f32[1], right.m128_f32[2], 0.0f,
        up.m128_f32[0], up.m128_f32[1], up.m128_f32[2], 0.0f,
        forward.m128_f32[0], forward.m128_f32[1], forward.m128_f32[2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // Setze die Rotation
    rotation = rotationMatrix;

}

float Transform::Distance(const DirectX::XMVECTOR& target)
{
    DirectX::XMVECTOR difference = DirectX::XMVectorSubtract(target, position);
    DirectX::XMVECTOR squaredDifference = DirectX::XMVector3LengthSq(difference);
    return sqrt(DirectX::XMVectorGetX(squaredDifference) + DirectX::XMVectorGetY(squaredDifference) + DirectX::XMVectorGetZ(squaredDifference));
}

float Transform::ExtractRoll(const DirectX::XMMATRIX* XMMatrix_p_Rotation)
{
    DirectX::XMFLOAT4X4 XMFLOAT4X4_Values;
    DirectX::XMStoreFloat4x4(&XMFLOAT4X4_Values, DirectX::XMMatrixTranspose(*XMMatrix_p_Rotation));

    return (float)atan2(XMFLOAT4X4_Values._21, XMFLOAT4X4_Values._22);
}

DirectX::XMMATRIX Transform::GetLocalTransformationMatrix()
{
    return scaling * rotation * translation;
}









