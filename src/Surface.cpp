#include "Surface.h"

Surface::Surface() :
    size_position(0),
    size_listPosition(0),
    size_color(0),
    size_listColor(0),
    size_listIndex(0),
    size_normal(0),
    size_listNormal(0),
    positionBuffer(nullptr),
    colorBuffer(nullptr),
    indexBuffer(nullptr),
    normalBuffer(nullptr),
    uv1Buffer(nullptr),
    uv2Buffer(nullptr),
    size_listUV1(0),
    size_listUV2(0),
    size_uv1(0),
    size_uv2(0),
    pMesh(nullptr),
    isActive(true),
    test(false)

{
}

Surface::~Surface() {
    isActive = false;
    Memory::SafeRelease(positionBuffer);
    Memory::SafeRelease(colorBuffer);
    Memory::SafeRelease(indexBuffer);
    Memory::SafeRelease(normalBuffer);      
    Memory::SafeRelease(uv1Buffer);         
    Memory::SafeRelease(uv2Buffer);
}

float Surface::getVertexX(unsigned int index) const
{
    if (index >= position.size())
        return 0.0f;
    return position[index].x;
}

float Surface::getVertexY(unsigned int index) const
{
    if (index >= position.size())
        return 0.0f;
    return position[index].y;
}

float Surface::getVertexZ(unsigned int index) const
{
    if (index >= position.size())
        return 0.0f;
    return position[index].z;
}

void Surface::AddVertex(unsigned int index, float x, float y, float z)
{
    if (index < position.size()) {          
        position[index] = DirectX::XMFLOAT3(x, y, z);
    }
    else {
        position.push_back(DirectX::XMFLOAT3(x, y, z));
    }
    size_listPosition = (unsigned int)position.size();
    size_position = sizeof(DirectX::XMFLOAT3);
}

void Surface::VertexColor(unsigned int index, float r, float g, float b)
{
    if (index < color.size()) {             
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
    if (index < normal.size()) {            
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
    if (index < uv1.size()) {               // ← FIXED: removed >= 0 check
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

void Surface::Draw(const gdx::CDevice* device, const DWORD flagsVertex)
{
    unsigned int offset = 0;
    unsigned int cnt = 0;

    if (flagsVertex & D3DVERTEX_POSITION) {
        device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &positionBuffer, &size_position, &offset);
        cnt++;
    }
    if (flagsVertex & D3DVERTEX_NORMAL) {
        device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &normalBuffer, &size_normal, &offset);
        cnt++;
    }
    if (flagsVertex & D3DVERTEX_COLOR) {
        device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &colorBuffer, &size_color, &offset);
        cnt++;
    }
    if (flagsVertex & D3DVERTEX_TEX1) {
        device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &uv1Buffer, &size_uv1, &offset);
        cnt++;
    }
    if (flagsVertex & D3DVERTEX_TEX2) {
        device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &uv2Buffer, &size_uv2, &offset);
        cnt++;
    }

    device->GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    if (!test)
    {
        device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        device->GetDeviceContext()->DrawIndexed(size_listIndex, 0, 0);
    }
    else
    {
        device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        device->GetDeviceContext()->Draw(size_listIndex, 0);  // ← Bleibt Draw()
    }
}

void Surface::CalculateSize(XMMATRIX rotationMatrix, XMFLOAT3& minSize, XMFLOAT3& maxSize)
{
    minPoint.x = FLT_MAX;
    minPoint.y = FLT_MAX;
    minPoint.z = FLT_MAX;
    maxPoint.x = -FLT_MAX;
    maxPoint.y = -FLT_MAX;
    maxPoint.z = -FLT_MAX;

    for (const auto& vertex : position)
    {
        XMVECTOR rotatedVertex = XMVector3Transform(XMLoadFloat3(&vertex), rotationMatrix);
        XMFLOAT3 transformedCoords;
        XMStoreFloat3(&transformedCoords, rotatedVertex);

        if (transformedCoords.x < minPoint.x)
            minPoint.x = transformedCoords.x;
        if (transformedCoords.y < minPoint.y)
            minPoint.y = transformedCoords.y;
        if (transformedCoords.z < minPoint.z)
            minPoint.z = transformedCoords.z;

        if (transformedCoords.x > maxPoint.x)
            maxPoint.x = transformedCoords.x;
        if (transformedCoords.y > maxPoint.y)
            maxPoint.y = transformedCoords.y;
        if (transformedCoords.z > maxPoint.z)
            maxPoint.z = transformedCoords.z;
    }

    minSize = minPoint;
    maxSize = maxPoint;
}
