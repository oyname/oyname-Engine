#pragma once
#include <list>
#include <d3d11.h>
#include <DirectXMath.h>
#include "gdxutil.h"
#include "Surface.h"  
#include "Transform.h"

__declspec(align(16))
struct MatrixSet
{
    DirectX::XMMATRIX viewMatrix;
    DirectX::XMMATRIX projectionMatrix;
    DirectX::XMMATRIX worldMatrix;
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void UpdateConstantBuffer(const gdx::CDevice* device, const DirectX::XMMATRIX view, const DirectX::XMMATRIX proj);
    void Update();

    bool isActive;
    
    Transform transform;

    MatrixSet cb;

    std::list<Surface*>* surfaces;

    ID3D11Buffer* constantBuffer;

    void* pShader;
    void* pBrush;

    void* operator new(size_t size) {
        // Ausrichtung auf 16 Bytes 
        return _aligned_malloc(size, 16);
    }

    void operator delete(void* p) noexcept {
        // richtigen Speicherdeallokator
        _aligned_free(p);
    }

private:

};

typedef Mesh* LPMESH;
typedef Mesh* LPCAMERA;
typedef Mesh MESH;
typedef Mesh CAMERA;



