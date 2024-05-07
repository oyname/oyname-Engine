#include "Memory.h"
#include "Mesh.h"

Mesh::Mesh() :
    isActive(false),
    constantBuffer(nullptr),
    surfaces(nullptr),
    pShader(nullptr),
    pBrush(nullptr)
{
    transform.setWorldMatrix(&this->matrixSet.worldMatrix);
}

Mesh::~Mesh() {
    Memory::SafeRelease(constantBuffer);
}

void Mesh::Update(const gdx::CDevice* device, MatrixSet* matrixSet)
{
    HRESULT hr = S_OK;
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    (*matrixSet).worldMatrix = transform.getScaling() * transform.getRotation() * transform.getTranslation();

    hr = device->GetDeviceContext()->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        return;
    }

    memcpy(mappedResource.pData, matrixSet, sizeof(MatrixSet));
    device->GetDeviceContext()->Unmap(constantBuffer, 0);

    device->GetDeviceContext()->VSSetConstantBuffers(0, 1, &constantBuffer);
    device->GetDeviceContext()->PSSetConstantBuffers(0, 1, &constantBuffer);
}