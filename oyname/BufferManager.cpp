#include "BufferManager.h"

BufferManager::BufferManager() : m_device(nullptr) {}

void BufferManager::Init(ID3D11Device* device)
{
    m_device = device;
}

HRESULT BufferManager::CreateBuffer(const void* data, UINT size, UINT count, D3D11_BIND_FLAG bindFlags, ID3D11Buffer** buffer)
{
    // Buffer Beschreibung
    D3D11_BUFFER_DESC bufferDesc = {};
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = (bindFlags & D3D11_BIND_CONSTANT_BUFFER) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = size * count;
    bufferDesc.BindFlags = bindFlags;
    bufferDesc.CPUAccessFlags = (bindFlags & D3D11_BIND_CONSTANT_BUFFER) ? D3D11_CPU_ACCESS_WRITE : 0;
    bufferDesc.MiscFlags = 0;

    // Initialisierungsdaten
    D3D11_SUBRESOURCE_DATA initData = {};
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = data;

    // Buffer erstellen
    return m_device->CreateBuffer(&bufferDesc, &initData, buffer);
}

unsigned int BufferManager::AddVertex(SURFACE* surface, float x, float y, float z) 
{
    surface->position.push_back(DirectX::XMFLOAT3(x, y, z));
    surface->size_listVertices = (unsigned int)surface->position.size();
    surface->size_vertex = sizeof(DirectX::XMFLOAT3);
    return (unsigned int)surface->position.size();
}

unsigned int BufferManager::AddColor(SURFACE* surface, float r, float g, float b)
{
    surface->color.push_back(DirectX::XMFLOAT4(r, g, b, 1.0f));
    surface->size_listColor = (unsigned int)surface->color.size();
    surface->size_color = sizeof(DirectX::XMFLOAT4);
    return (unsigned int)surface->color.size();
}

// Funktion zum Hinzufügen eines Index
unsigned int BufferManager::AddIndex(SURFACE* surface, UINT index) {
    surface->indices.push_back(index);
    surface->size_listIndex = (unsigned int)surface->indices.size();
    return (unsigned int)surface->indices.size();
}

