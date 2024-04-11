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

    void* operator new(size_t size) {
        // Ausrichtung auf 16 Bytes 
        return _aligned_malloc(size, 16);
    }

    void operator delete(void* p) noexcept {
        // richtigen Speicherdeallokator
        _aligned_free(p);
    }
};
typedef Brush* LPBRUSH;
typedef Brush BRUSH;
