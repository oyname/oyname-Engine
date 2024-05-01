#include "Surface.h"

Surface::Surface() :
    isActive(false),
    size_vertex(0),
    size_listVertices(0),
    size_color(0),
    size_listColor(0),
    size_listIndex(0),
    size_normal(0),
    size_listNormal(0),
    positionBuffer(nullptr),
    colorBuffer(nullptr),
    indexBuffer(nullptr),
    normalBuffer(nullptr),
    pShader(nullptr)
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

float Surface::getVertexX(unsigned int index) const
{
    if (index < 0 || index >= position.size())
        return 0.0f;

    return position[index].x;
}

float Surface::getVertexY(unsigned int index) const
{
    if (index < 0 || index >= position.size())
        return 0.0f;

    return position[index].y;
}

float Surface::getVertexZ(unsigned int index) const
{
    if (index < 0 || index >= position.size())
        return 0.0f;

    return position[index].z;
}

void Surface::VertexColor(unsigned int index, float r, float g, float b)
{
    if (index >= 0 && index < color.size()) {
        color[index] = DirectX::XMFLOAT4(r, g, b, 1.0f);
    }
    else {
        color.push_back(DirectX::XMFLOAT4(r, g, b, 1.0f));
    }
    size_listColor = static_cast<unsigned int>(color.size());
    size_color = sizeof(DirectX::XMFLOAT4);
}

void Surface::VertexNormal(unsigned int index, float x, float y, float z)
{
    if (index >= 0 && index < normal.size()) {
        normal[index] = DirectX::XMFLOAT3(x, y, z);
    }
    else {
        normal.push_back(DirectX::XMFLOAT3(x, y, z));
    }
    size_listNormal = (unsigned int)normal.size();
    size_normal = sizeof(DirectX::XMFLOAT3);
}

void Surface::VertexTexCoords(unsigned int index, float u, float v)
{
    if (index >= 0 && index < uv1.size()) {
        uv1[index] = DirectX::XMFLOAT2(u, v);
    }
    else {
        uv1.push_back(DirectX::XMFLOAT2(u, v));
    }
    size_listUV1 = (unsigned int)uv1.size();
    size_uv1 = sizeof(DirectX::XMFLOAT2);
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
    if (flagsVertex & D3DVERTEX_NORMAL) {
        device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &normalBuffer, &size_normal, &offset);
        cnt++;
    }
    if (flagsVertex & D3DVERTEX_COLOR) {
        device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &colorBuffer,   &size_color, &offset);
        cnt++;
    }
    if (flagsVertex & D3DVERTEX_TEX1) {
        device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &uv1Buffer,  &size_uv1, &offset);
        cnt++;
    }
    if (flagsVertex & D3DVERTEX_TEX2) {
        device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &uv2Buffer,  &size_uv2, &offset);
        cnt++;
    }

    device->GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    device->GetDeviceContext()->DrawIndexed(size_listIndex, 0, 0);
}

void Surface::CalculateObjectSize()
{
    float minX = FLT_MAX;
    float minY = FLT_MAX;
    float minZ = FLT_MAX;
    float maxX = -FLT_MAX;
    float maxY = -FLT_MAX;
    float maxZ = -FLT_MAX;

    // Durchlaufe alle Vertices des Objekts und aktualisiere die minimalen und maximalen Koordinaten
    for (const auto& vertex : position) {
        if (vertex.x < minX)
            minX = vertex.x;
        if (vertex.y < minY)
            minY = vertex.y;
        if (vertex.z < minZ)
            minZ = vertex.z;

        if (vertex.x > maxX)
            maxX = vertex.x;
        if (vertex.y > maxY)
            maxY = vertex.y;
        if (vertex.z > maxZ)
            maxZ = vertex.z;
    }

    // Berechne die Größe in X, Y und Z
    sizeX = maxX - minX;
    sizeY = maxY - minY;
    sizeZ = maxZ - minZ;
}
