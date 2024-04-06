#pragma once

#include <list>
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include "Mesh.h"   

class Shader;

class Brush 
{
public:
    Brush();
    ~Brush();

    bool isActive;
    float shininess;
    float transparency;

    DirectX::XMFLOAT4 diffuseColor;
    DirectX::XMFLOAT4 specularColor;

    ID3D11ShaderResourceView* pTexture;

    std::list<Mesh*>* meshes;

    Shader* pShader;
};

typedef Brush* LPBRUSH;
typedef Brush BRUSH;
