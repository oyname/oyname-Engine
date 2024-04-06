#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "gdxutil.h" 

class Shader;

class Surface {
public:
    Surface();
    ~Surface();

    bool isActive;
    std::vector<DirectX::XMFLOAT3> position;
    unsigned int size_vertex;
    unsigned int size_listVertices;

    std::vector<DirectX::XMFLOAT4> color;
    unsigned int size_color;
    unsigned int size_listColor;

    std::vector<unsigned int> indices;
    unsigned int size_listIndex;

    ID3D11Buffer* positionBuffer;
    ID3D11Buffer* colorBuffer;
    ID3D11Buffer* indexBuffer;

    Shader* pShader;
};

typedef Surface* LPSURFACE;
typedef Surface SURFACE;