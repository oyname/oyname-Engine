#include "RenderManager.h"

RenderManager::RenderManager(ObjectManager& objectManager) : m_objectManager(objectManager) {

}

void RenderManager::RenderLoop()
{
    RenderMesh();
}

void RenderManager::RenderMesh()
{
    m_objectManager.m_device->ClearRenderTargetDepthStencil();

    // Geht alle Shader durch und rendert mit jedem Shader alle Objekte
    for (const auto& shader : m_objectManager.m_shaders) 
    {
        shader->isActive = true;
        m_objectManager.m_device->GetDeviceContext()->VSSetShader(shader->vertexShader, nullptr, 0);
        m_objectManager.m_device->GetDeviceContext()->PSSetShader(shader->pixelShader, nullptr, 0);
        m_objectManager.m_device->GetDeviceContext()->IASetInputLayout(shader->inputlayout);

        for (const auto& brush : *(shader->brushes)) 
        {
            for (const auto& mesh : *(brush->meshes)) 
            {
                m_objectManager.m_device->GetDeviceContext()->UpdateSubresource(mesh->constantBuffer, 0, nullptr, &mesh->cb, 0, 0);
                m_objectManager.m_device->GetDeviceContext()->VSSetConstantBuffers(0, 1, &mesh->constantBuffer);
                m_objectManager.m_device->GetDeviceContext()->PSSetConstantBuffers(0, 1, &mesh->constantBuffer);

                for (const auto& surface : *(mesh->surfaces)) 
                {
                    unsigned int offset = 0;
                    unsigned int cnt = 0;

                    if (shader->flags & D3DVERTEX_POSITION) 
                    {
                        m_objectManager.m_device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &surface->positionBuffer, &surface->size_vertex, &offset);
                        cnt++;
                    }
                    if (shader->flags & D3DVERTEX_COLOR) 
                    {
                        m_objectManager.m_device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &surface->colorBuffer, &surface->size_color, &offset);
                        cnt++;
                    }
                    m_objectManager.m_device->GetDeviceContext()->IASetIndexBuffer(surface->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

                    m_objectManager.m_device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    m_objectManager.m_device->GetDeviceContext()->DrawIndexed(surface->size_listIndex, 0, 0);
                }
            }
        }
    }
}



