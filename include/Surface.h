#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "gdxutil.h"
#include "gdxdevice.h"

using namespace DirectX;

class Mesh;    // forward

class Surface {
public:
    Surface();
    ~Surface();

    void AddVertex(unsigned int index, float x, float y, float z);
    void VertexNormal(unsigned int index, float x, float y, float z);
    void VertexColor(unsigned int index, float r, float g, float b);
    void VertexTexCoords(unsigned int index, float u, float v);
    void AddIndex(UINT index);

    void Draw(const gdx::CDevice* m_device, const DWORD flags);

    // Getter
    float getVertexX(unsigned int index) const;
    float getVertexY(unsigned int index) const;
    float getVertexZ(unsigned int index) const;

    void CalculateSize(XMMATRIX roationMatrix, XMFLOAT3& minSize, XMFLOAT3& maxSize);

public:
    bool isActive = false;

    std::vector<XMFLOAT3> position;
    unsigned int size_position;
    unsigned int size_listPosition;

    std::vector<XMFLOAT3> normal;
    unsigned int size_normal;
    unsigned int size_listNormal;

    std::vector<XMFLOAT4> color;
    unsigned int size_color;
    unsigned int size_listColor;

    std::vector<XMFLOAT2> uv1;
    unsigned int size_uv1;
    unsigned int size_listUV1;

    std::vector<XMFLOAT2> uv2;
    unsigned int size_uv2;
    unsigned int size_listUV2;

    std::vector<unsigned int> indices;
    unsigned int size_listIndex;

    ID3D11Buffer* positionBuffer;
    ID3D11Buffer* normalBuffer;
    ID3D11Buffer* colorBuffer;
    ID3D11Buffer* uv1Buffer;
    ID3D11Buffer* uv2Buffer;
    ID3D11Buffer* indexBuffer;

    Mesh* pMesh = nullptr;               
    XMFLOAT3 minPoint;
    XMFLOAT3 maxPoint;

public:
    // ist dafür gemacht um Linien zu rendern!
    bool test;
};

typedef Surface* LPSURFACE;
typedef Surface SURFACE;
