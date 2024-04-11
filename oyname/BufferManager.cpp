#include "BufferManager.h"

// siehe InputLayoutManager.cpp
// 
// InputLayoutManager erstellt ein Input Layout basierend auf den angegebenen Flags und dem Shader-Objekt 
// und gibt den HRESULT-Wert zurück, um den Erfolg oder das Scheitern des Vorgangs anzuzeigen.

BufferManager::BufferManager() : m_device(nullptr) {}

void BufferManager::Init(ID3D11Device* device)
{
    m_device = device;
}

HRESULT BufferManager::CreateBuffer(const void* data, UINT size, UINT count, D3D11_BIND_FLAG bindFlags, ID3D11Buffer** buffer)
{
    // Buffer Beschreibung
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = (bindFlags & D3D11_BIND_CONSTANT_BUFFER) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = size * count;
    bufferDesc.BindFlags = bindFlags;
    bufferDesc.CPUAccessFlags = (bindFlags & D3D11_BIND_CONSTANT_BUFFER) ? D3D11_CPU_ACCESS_WRITE : 0;
    bufferDesc.MiscFlags = 0;

    // Initialisierungsdaten
    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = data;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    // Buffer erstellen
    return m_device->CreateBuffer(&bufferDesc, &initData, buffer);
}

