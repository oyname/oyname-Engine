#include "BufferManager.h"

// See InputLayoutManager.cpp
// InputLayoutManager creates an input layout based on the specified flags and the shader object
// and returns the HRESULT value to indicate the success or failure of the operation.

BufferManager::BufferManager() : m_device(nullptr), m_context(nullptr) {}

void BufferManager::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    m_device = device;
    m_context = context;
}

HRESULT BufferManager::CreateBuffer(const void* data, UINT size, UINT count, D3D11_BIND_FLAG bindFlags, ID3D11Buffer** buffer)
{
    // Buffer Description
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = (bindFlags & D3D11_BIND_CONSTANT_BUFFER) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = size * count;
    bufferDesc.BindFlags = bindFlags;
    bufferDesc.CPUAccessFlags = (bindFlags & D3D11_BIND_CONSTANT_BUFFER) ? D3D11_CPU_ACCESS_WRITE : 0;
    bufferDesc.MiscFlags = 0;

    // Initialization Data
    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = data;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    // Create Buffer
    return m_device->CreateBuffer(&bufferDesc, &initData, buffer);
}

void BufferManager::UpdateBuffer(ID3D11Buffer* buffer, const void* data, UINT dataSize)
{
    m_context->UpdateSubresource(buffer, 0, nullptr, data, dataSize, 0);

}

HRESULT BufferManager::UpdateConstantBuffer(ID3D11Buffer* buffer, const void* data, UINT dataSize)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = m_context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        return hr;
    }
    
    memcpy(mappedResource.pData, data, dataSize);
    m_context->Unmap(buffer, 0);

    return hr;
}


