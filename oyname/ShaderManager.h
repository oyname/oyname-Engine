#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <list>
#include "ObjectManager.h"
#include "gdxutil.h"

class ShaderManager {
public:
    ShaderManager();

    void Init(ID3D11Device* device);
    HRESULT  CreateShader(SHADER* shader, const std::wstring& vertexShaderFile, const std::wstring& pixelShaderFile);
    LPSHADER GetShader();
    void SetShader(LPSHADER shader);
    void SetShaderFolder(const std::wstring& shaderFolder);

private:
    ID3D11Device* m_device;
    ObjectManager* m_objectManager; // Referenz auf den ObjectManager
    LPSHADER m_standardShader;
    std::wstring m_shaderfolder;
};
