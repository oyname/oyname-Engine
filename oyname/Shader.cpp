#include "Shader.h"

Shader::Shader() :
    isActive(false),
    flagsVertex(0),
    bytecodeVS(nullptr),
    sizeVS(0),
    inputlayoutVertex(nullptr),
    vertexShader(nullptr),
    pixelShader(nullptr),
    blobVS(nullptr),
    blobPS(nullptr),
    brushes(nullptr)
{
    // 
}

Shader::~Shader() {
    Memory::SafeRelease(inputlayoutVertex);
    Memory::SafeRelease(vertexShader);
    Memory::SafeRelease(pixelShader);
    Memory::SafeRelease(blobVS);
    Memory::SafeRelease(blobPS);
}

void Shader::UpdateShader(const gdx::CDevice* device)
{
    // InputLayout für Vertices setzen
    device->GetDeviceContext()->IASetInputLayout(inputlayoutVertex);

    // Vertex und Pixelshader setzen
    device->GetDeviceContext()->VSSetShader(vertexShader, nullptr, 0);
    device->GetDeviceContext()->PSSetShader(pixelShader, nullptr, 0);
}
