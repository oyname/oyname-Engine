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
