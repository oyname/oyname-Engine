#pragma once

#include <list>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include "gdxutil.h"
#include "Brush.h"    

class Shader {
public:
    Shader();
    ~Shader();

    bool isActive;
    DWORD flags;
    void* bytecodeVS;
    unsigned int sizeVS;
    std::wstring vertexShaderFile;
    std::wstring pixelShaderFile;

    ID3D11InputLayout* inputlayout;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D10Blob* blobVS;
    ID3D10Blob* blobPS;

    std::list<Brush*>* brushes;
};

typedef Shader* LPSHADER;
typedef Shader SHADER;

