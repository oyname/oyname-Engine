#include "Mesh.h"
#include "Memory.h" // Assuming Memory::SafeRelease is defined here

Mesh::Mesh() :
    isActive(false),
    constantBuffer(nullptr),
    surfaces(nullptr),
    pShader(nullptr),
    position(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
    lookAt(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
    up(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),
    right(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)),
    mRotate(DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(0.0f)))
{
    cb.projectionMatrix = DirectX::XMMatrixIdentity();
    cb.viewMatrix = DirectX::XMMatrixIdentity();
    cb.worldMatrix = DirectX::XMMatrixIdentity();

    mRotate = DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMConvertToRadians(0.0f));
    mTranslation = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
    mScaling = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
}

Mesh::~Mesh() {
    Memory::SafeRelease(constantBuffer);
}

void Mesh::TurnEntity(float fRotateX, float fRotateY, float fRotateZ, Space mode) 
{
    // Berechnung der Rotationsmatrizen
    DirectX::XMMATRIX rotationX = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(fRotateX));
    DirectX::XMMATRIX rotationY = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(fRotateY));
    DirectX::XMMATRIX rotationZ = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(fRotateZ));

    // Erstellen einer Matrix, die die Rotationen kombiniert
    DirectX::XMMATRIX rotationMatrix = rotationX * rotationY * rotationZ;

    // Je nach Modus die Rotation auf die entsprechende Matrix anwenden
    if (mode == Space::Local) {
        // Lokale Rotation
        mRotate = rotationMatrix * mRotate;
    }
    else if (mode == Space::World) {
        // Globale Rotation
        mRotate *= rotationMatrix;
    }

    // Transformieren der lokalen Achsen entsprechend der globalen Rotation
    lookAt = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(defaultLookAt, mRotate));
    up = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(defaultUp, mRotate));
    right = DirectX::XMVector3Cross(lookAt, up);
}

void Mesh::RotateEntity(float fRotateX, float fRotateY, float fRotateZ, Space mode)
{
    // Berechnung der Rotationsmatrizen
    DirectX::XMMATRIX rotationX = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(fRotateX));
    DirectX::XMMATRIX rotationY = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(fRotateY));
    DirectX::XMMATRIX rotationZ = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(fRotateZ));

    // Erstellen einer Matrix, die die Rotationen kombiniert
    DirectX::XMMATRIX rotationMatrix = rotationX * rotationY * rotationZ;

    // Je nach Modus die Rotation auf die entsprechende Matrix anwenden
    if (mode == Space::Local) {
        // Lokale Rotation
        mRotate = rotationMatrix;
    }
    else if (mode == Space::World) {
        // Globale Rotation
        mRotate = rotationMatrix * mRotate;
    }

    // Transformieren der lokalen Achsen entsprechend der globalen Rotation
    lookAt = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(defaultLookAt, mRotate));
    up = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(defaultUp, mRotate));
    right = DirectX::XMVector3Cross(lookAt, up);
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

void Mesh::ScaleEntity(float x, float y, float z)
{
    mScaling = DirectX::XMMatrixScaling(x, y, z); 
}

void Mesh::Update() {
    cb.viewMatrix = DirectX::XMMatrixLookToLH(position, lookAt, up);
}

void Mesh::UpdateConstantBuffer(const gdx::CDevice* device, const DirectX::XMMATRIX view, const DirectX::XMMATRIX proj)
{
    HRESULT hr = S_OK;
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    cb.viewMatrix = view;
    cb.projectionMatrix = proj;
    cb.worldMatrix = mScaling * mRotate * mTranslation;

    hr = device->GetDeviceContext()->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        return;
    }

    memcpy(mappedResource.pData, &cb, sizeof(MatrixSet));
    device->GetDeviceContext()->Unmap(constantBuffer, 0);

    device->GetDeviceContext()->VSSetConstantBuffers(0, 1, &constantBuffer);
    device->GetDeviceContext()->PSSetConstantBuffers(0, 1, &constantBuffer);
}