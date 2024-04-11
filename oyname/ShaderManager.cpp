#include "ShaderManager.h"
#include <fstream>

ShaderManager::ShaderManager() : m_device(nullptr), m_objectManager(nullptr)
{
    // Changed the location of precompiled files from $(OutDir)%(Filename).cso to $(ProjectDir)%(Filename).cso
    //
    SetShaderFolder(L"..\\oyname\\");
}

void ShaderManager::Init(ID3D11Device* device)
{
    m_device = device;
}

//HRESULT ShaderManager::CreateShader(SHADER* shader, const std::wstring& vertexShaderFile, const std::wstring& pixelShaderFile)
//{
//    HRESULT hr;
//
//    hr = D3DReadFileToBlob((m_shaderfolder + vertexShaderFile).c_str(), &shader->blobVS);
//    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
//        return hr;
//    }
//
//    hr = m_device->CreateVertexShader(shader->blobVS->GetBufferPointer(), shader->blobVS->GetBufferSize(), nullptr, &shader->vertexShader);
//    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
//        return hr;
//    }
//
//    hr = D3DReadFileToBlob((m_shaderfolder + pixelShaderFile).c_str(), &shader->blobPS);
//    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
//        return hr;
//    }
//
//    hr = m_device->CreatePixelShader(shader->blobPS->GetBufferPointer(), shader->blobPS->GetBufferSize(), nullptr, &shader->pixelShader);
//    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
//        return hr;
//    }
//
//    shader->vertexShaderFile = vertexShaderFile;
//    shader->pixelShaderFile = pixelShaderFile;
//
//    return S_OK;
//}

HRESULT ShaderManager::CreateShader(SHADER* shader, const std::wstring& vertexShaderFile, const std::wstring& pixelShaderFile, const std::string& entryPoint)
{
    HRESULT hr;

    // Load and compile the vertex shader
    hr = CompileShaderFromFile(m_shaderfolder + vertexShaderFile, "main", "vs_5_0", &shader->blobVS);
    if (FAILED(hr)) {
        return hr;
    }

    // Create the vertex shader
    hr = m_device->CreateVertexShader(shader->blobVS->GetBufferPointer(), shader->blobVS->GetBufferSize(), nullptr, &shader->vertexShader);
    if (FAILED(hr)) {
        return hr;
    }

    // Load and compile the pixel shader
    hr = CompileShaderFromFile(m_shaderfolder + pixelShaderFile, entryPoint, "ps_5_0", &shader->blobPS);
    if (FAILED(hr)) {
        return hr;
    }

    // Create the pixel shader
    hr = m_device->CreatePixelShader(shader->blobPS->GetBufferPointer(), shader->blobPS->GetBufferSize(), nullptr, &shader->pixelShader);
    if (FAILED(hr)) {
        return hr;
    }

    // Save the filenames
    shader->vertexShaderFile = vertexShaderFile;
    shader->pixelShaderFile = pixelShaderFile;

    return S_OK;
}

HRESULT ShaderManager::CompileShaderFromFile(const std::wstring& filename, const std::string& entryPoint, const std::string& shaderModel, ID3DBlob** blob)
{
    HRESULT hr;

    // Compile the shader from file
    hr = D3DCompileFromFile(filename.c_str(), nullptr, nullptr, entryPoint.c_str(), shaderModel.c_str(), 0, 0, blob, nullptr);
    if (FAILED(hr)) {
        return Debug::GetErrorMessage(__FILE__, __LINE__, hr);
    }

    return hr;
}


SHADER* ShaderManager::GetShader() {
    return m_standardShader;
}

void ShaderManager::SetShader(LPSHADER shader)
{
    m_standardShader = shader;
}

void ShaderManager::SetShaderFolder(const std::wstring& shaderFolder)
{
    m_shaderfolder = shaderFolder;
}

