#include "BufferManager.h"

// See InputLayoutManager.cpp
// InputLayoutManager creates an input layout based on the specified flags and the shader object
// and returns the HRESULT value to indicate the success or failure of the operation.

BufferManager::BufferManager() : m_device(nullptr) {}

void BufferManager::Init(ID3D11Device* device)
{
    m_device = device;
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


