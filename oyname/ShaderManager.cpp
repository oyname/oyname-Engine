#include "ShaderManager.h"
#include <fstream>

ShaderManager::ShaderManager() : m_device(nullptr), m_objectManager(nullptr) 
{
    // Speicherort der vorkomplilierten Dateien von $(OutDir)%(Filename).cso
    // in $(ProjectDir)%(Filename).cso geändert
    //
    SetShaderFolder(L"..\\oyname\\");
}

void ShaderManager::Init(ID3D11Device* device)
{
    m_device = device;
}

HRESULT ShaderManager::CreateShader(SHADER* shader, const std::wstring& vertexShaderFile, const std::wstring& pixelShaderFile)
{
    HRESULT hr;

    hr = D3DReadFileToBlob((m_shaderfolder + vertexShaderFile).c_str(), &shader->blobVS);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        return hr;
    }

    hr = m_device->CreateVertexShader(shader->blobVS->GetBufferPointer(), shader->blobVS->GetBufferSize(), nullptr, &shader->vertexShader);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        return hr;
    }

    hr = D3DReadFileToBlob((m_shaderfolder + pixelShaderFile).c_str(), &shader->blobPS);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        return hr;
    }

    hr = m_device->CreatePixelShader(shader->blobPS->GetBufferPointer(), shader->blobPS->GetBufferSize(), nullptr, &shader->pixelShader);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        return hr;
    }

    shader->vertexShaderFile = vertexShaderFile;
    shader->pixelShaderFile = pixelShaderFile;

    return S_OK;
}

SHADER* ShaderManager::GetStandardShader() {
    return m_standardShader;
}

void ShaderManager::SetStandardShader(LPSHADER shader)
{
    m_standardShader = shader;
}

void ShaderManager::SetShaderFolder(const std::wstring& shaderFolder)
{
    m_shaderfolder = shaderFolder;
}

