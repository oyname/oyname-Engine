#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>
#include "gdxutil.h"
#include "ObjectManager.h"

class InputLayoutManager {
public:
    InputLayoutManager();
    ~InputLayoutManager();

    void Init(ID3D11Device* device);
    HRESULT CreateInputLayout(SHADER* shader, DWORD flags);

private:
    ID3D11Device* m_device;
};