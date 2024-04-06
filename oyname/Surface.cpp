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

void Surface::AddIndex(UINT index)
{
    indices.push_back(index);
    size_listIndex = (unsigned int)indices.size();
}
