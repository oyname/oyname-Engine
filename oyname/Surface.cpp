#include "Surface.h"

Surface::Surface() :
    isActive(false),
    size_vertex(0),
    size_listVertices(0),
    size_color(0),
    size_listColor(0),
    size_listIndex(0),
    positionBuffer(nullptr),
    colorBuffer(nullptr),
    indexBuffer(nullptr)
{
}

Surface::~Surface() {
    Memory::SafeRelease(positionBuffer);
    Memory::SafeRelease(colorBuffer);
    Memory::SafeRelease(indexBuffer);
}

void Surface::AddVertex(float x, float y, float z)
{
    position.push_back(DirectX::XMFLOAT3(x, y, z));
    size_listVertices = (unsigned int)position.size();
    size_vertex = sizeof(DirectX::XMFLOAT3);
}

void Surface::VertexColor(float r, float g, float b)
{
    color.push_back(DirectX::XMFLOAT4(r, g, b, 1.0f));
    size_listColor = (unsigned int)color.size();
    size_color = sizeof(DirectX::XMFLOAT4);
}

void Surface::VertexNormal(float x, float y, float z)
{
    normal.push_back(DirectX::XMFLOAT3(x, y, z));
    size_listNormal = (unsigned int)normal.size();
    size_normal = sizeof(DirectX::XMFLOAT3);
}

void Surface::AddIndex(UINT index)
{
    indices.push_back(index);
    size_listIndex = (unsigned int)indices.size();
}

void Surface::Draw(const gdx::CDevice* device,const DWORD flagsVertex)
{
    unsigned int offset = 0;
    unsigned int cnt = 0;

    if (flagsVertex & D3DVERTEX_POSITION) {
        device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &positionBuffer,&size_vertex, &offset);
        cnt++;
    }
    if (flagsVertex & D3DVERTEX_COLOR) {
        device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &colorBuffer,   &size_color, &offset);
        cnt++;
    }
    if (flagsVertex & D3DVERTEX_NORMAL) {
        device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &normalBuffer,  &size_normal, &offset);
        cnt++;
    }
    if (flagsVertex & D3DVERTEX_TEX1) {
        device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &normalBuffer,  &size_normal, &offset);
        cnt++;
    }
    if (flagsVertex & D3DVERTEX_TEX2) {
        device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &normalBuffer,  &size_normal, &offset);
        cnt++;
    }

    device->GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    device->GetDeviceContext()->DrawIndexed(size_listIndex, 0, 0);
}
