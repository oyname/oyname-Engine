#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "gdxutil.h" 
#include "gdxdevice.h"

class Shader;

class Surface {
public:
    Surface();
    ~Surface();

public:
    void AddVertex(float x, float y, float z);
    void VertexNormal(float x, float y, float z);
    void VertexColor(float r, float g, float b);
    void AddIndex(UINT index);

    void Draw(const gdx::CDevice* m_device, const DWORD flags);

    bool isActive;

    std::vector<DirectX::XMFLOAT3> position;
    unsigned int size_vertex;
    unsigned int size_listVertices;

    std::vector<DirectX::XMFLOAT4> color;
    unsigned int size_color;
    unsigned int size_listColor;

    std::vector<DirectX::XMFLOAT3> normal;
    unsigned int size_normal;
    unsigned int size_listNormal;

    std::vector<unsigned int> indices;
    unsigned int size_listIndex;

    ID3D11Buffer* positionBuffer;
    ID3D11Buffer* normalBuffer;
    ID3D11Buffer* colorBuffer;
    ID3D11Buffer* indexBuffer;

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

typedef Surface* LPSURFACE;
typedef Surface SURFACE;