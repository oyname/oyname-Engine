#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "gdxutil.h" 
#include "gdxdevice.h"

class Surface {
public:
    Surface();
    ~Surface();

public:
    void AddVertex(float x, float y, float z);
    void VertexNormal(unsigned int index, float x, float y, float z);
    void VertexColor(unsigned int index, float r, float g, float b);
    void VertexTexCoords(unsigned int index, float u, float v);
    void AddIndex(UINT index);
    
    void Draw(const gdx::CDevice* m_device, const DWORD flags);

    bool isActive;

    std::vector<DirectX::XMFLOAT3> position;
    unsigned int size_vertex;
    unsigned int size_listVertices;

    std::vector<DirectX::XMFLOAT3> normal;
    unsigned int size_normal;
    unsigned int size_listNormal;
    
    std::vector<DirectX::XMFLOAT4> color;
    unsigned int size_color;
    unsigned int size_listColor;

    std::vector<DirectX::XMFLOAT2> uv1;
    unsigned int size_uv1;
    unsigned int size_listUV1;

    std::vector<DirectX::XMFLOAT2> uv2;
    unsigned int size_uv2;
    unsigned int size_listUV2;

    std::vector<unsigned int> indices;
    unsigned int size_listIndex;

    // Getter
    float getVertexX(unsigned int index) const;
    float getVertexY(unsigned int index) const;
    float getVertexZ(unsigned int index) const;

    float getSizeX() {return sizeX;};
    float getSizeY() {return sizeY;};
    float getSizeZ() {return sizeZ;};

    void CalculateObjectSize();

    //
    ID3D11Buffer* positionBuffer;
    ID3D11Buffer* normalBuffer;
    ID3D11Buffer* colorBuffer;
    ID3D11Buffer* uv1Buffer;
    ID3D11Buffer* uv2Buffer;
    ID3D11Buffer* indexBuffer;

    void* pShader;

    void* operator new(size_t size) {
        // Ausrichtung auf 16 Bytes 
        return _aligned_malloc(size, 16); 
    }

    void operator delete(void* p) noexcept {
        // richtigen Speicherdeallokator
        _aligned_free(p);
    }

public:
    float sizeX;
    float sizeY;
    float sizeZ;
};

typedef Surface* LPSURFACE;
typedef Surface SURFACE;