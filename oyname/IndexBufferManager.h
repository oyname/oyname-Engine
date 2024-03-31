#pragma once

#include <d3d11.h>
#include <vector>
#include "gdxutil.h"
#include "ObjectManager.h"

class IndexBufferManager {
private:
    ID3D11Device* m_device;

public:
    IndexBufferManager();
    ~IndexBufferManager();

    void Init(ID3D11Device* device);

    HRESULT CreateIndexBuffer(const void* indexData, UINT sizeList, ID3D11Buffer** Buffer);

    unsigned int AddIndex(SURFACE* surface, UINT index);
};