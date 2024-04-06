#include "Shader.h"

Shader::Shader() :
    isActive(false),
    flags(0),
    bytecodeVS(nullptr),
    sizeVS(0),
    inputlayout(nullptr),
    vertexShader(nullptr),
    pixelShader(nullptr),
    blobVS(nullptr),
    blobPS(nullptr),
    brushes(nullptr)
{
    // 
}

Shader::~Shader() {
    Memory::SafeRelease(inputlayout);
    Memory::SafeRelease(vertexShader);
    Memory::SafeRelease(pixelShader);
    Memory::SafeRelease(blobVS);
    Memory::SafeRelease(blobPS);
}
