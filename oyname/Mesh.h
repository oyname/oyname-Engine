#pragma once
#include <list>
#include <d3d11.h>
#include <DirectXMath.h>
#include "gdxutil.h"
#include "Surface.h"  
#include "Transform.h"
#include "Camera.h"

class Mesh : public Camera
{
public:
    Mesh();
    ~Mesh();

    void Update(const gdx::CDevice* device, MatrixSet* cb);

    bool isActive;
    
    Transform transform;

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

typedef Mesh* LPENTITY;
typedef Mesh ENTITY;



