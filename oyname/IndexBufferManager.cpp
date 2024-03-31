#include "IndexBufferManager.h"


IndexBufferManager::IndexBufferManager() : m_device(nullptr) {}

IndexBufferManager::~IndexBufferManager() {}

void IndexBufferManager::Init(ID3D11Device* device) {
    m_device = device;
}

HRESULT IndexBufferManager::CreateIndexBuffer(const void* indexData, UINT sizeList, ID3D11Buffer** Buffer) {
    // Buffer Beschreibung
    D3D11_BUFFER_DESC bufferDesc = {};
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = 4 * sizeList;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    // Initialisierungsdaten
    D3D11_SUBRESOURCE_DATA initData = {};
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = indexData;

    // Buffer erstellen
    return m_device->CreateBuffer(&bufferDesc, &initData, Buffer);
}

// Funktion zum Hinzufügen eines Index
unsigned int IndexBufferManager::AddIndex(SURFACE* surface, UINT index) {
    surface->indices.push_back(index);
    surface->indexCount = surface->indices.size();
    return surface->indices.size();
}
