#include "ShaderManager.h"
#include <fstream>

ShaderManager::ShaderManager() : m_device(nullptr), m_objectManager(nullptr)
{

}

void ShaderManager::Init(ID3D11Device* device)
{
    m_device = device;
}

HRESULT ShaderManager::CreateShader(SHADER* shader, const std::wstring& vertexShaderFile, const std::string& vertexEntryPoint, const std::wstring& pixelShaderFile, const std::string& pixelEntryPoint)
{
    HRESULT hr;

    // Load and compile the vertex shader
    hr = CompileShaderFromFile(SHADER_FOLDER + vertexShaderFile, vertexEntryPoint, "vs_5_0", &shader->blobVS);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        return hr;
    }

    // Create the vertex shader
    hr = m_device->CreateVertexShader(shader->blobVS->GetBufferPointer(), shader->blobVS->GetBufferSize(), nullptr, &shader->vertexShader);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        return hr;
    }

    // Load and compile the pixel shader
    hr = CompileShaderFromFile(SHADER_FOLDER + pixelShaderFile, pixelEntryPoint, "ps_5_0", &shader->blobPS);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        return hr;
    }

    // Create the pixel shader
    hr = m_device->CreatePixelShader(shader->blobPS->GetBufferPointer(), shader->blobPS->GetBufferSize(), nullptr, &shader->pixelShader);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
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


