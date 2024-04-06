#include "gdxengine.h"
#include "RenderManager.h"

RenderManager::RenderManager(MeshManager& meshManager) : m_meshManager(meshManager) {

}

void RenderManager::RenderLoop()
{
    RenderMesh();
}

void RenderManager::RenderMesh()
{
    m_meshManager.m_device->ClearRenderTargetDepthStencil();

    // Geht alle Shader durch und rendert mit jedem Shader alle Objekte
    for (const auto& shader : m_meshManager.m_shaders)
    {
        shader->isActive = true;
        m_meshManager.m_device->GetDeviceContext()->VSSetShader(shader->vertexShader, nullptr, 0);
        m_meshManager.m_device->GetDeviceContext()->PSSetShader(shader->pixelShader, nullptr, 0);
        m_meshManager.m_device->GetDeviceContext()->IASetInputLayout(shader->inputlayout);

        for (const auto& brush : *(shader->brushes))
        {
            for (const auto& mesh : *(brush->meshes))
            {
                mesh->cb.viewMatrix = m_meshManager.m_engine->GetCam().GetCurrentCam()->cb.viewMatrix;
                mesh->cb.projectionMatrix = m_meshManager.m_engine->GetCam().GetCurrentCam()->cb.projectionMatrix;
                mesh->cb.worldMatrix = mesh->mRotate * mesh->mTranslation;
                
                // Konstanten in den Constant Buffer schreiben
                D3D11_MAPPED_SUBRESOURCE mappedResource;
                m_meshManager.m_device->GetDeviceContext()->Map(mesh->constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
                memcpy(mappedResource.pData, &mesh->cb, sizeof(ConstantBuffer));
                m_meshManager.m_device->GetDeviceContext()->Unmap(mesh->constantBuffer, 0);

                //m_objectManager.m_device->GetDeviceContext()->UpdateSubresource(mesh->constantBuffer, 0, nullptr, &mesh->cb, 0, 0); // alte Version
                m_meshManager.m_device->GetDeviceContext()->VSSetConstantBuffers(0, 1, &mesh->constantBuffer);
                m_meshManager.m_device->GetDeviceContext()->PSSetConstantBuffers(0, 1, &mesh->constantBuffer);

                for (const auto& surface : *(mesh->surfaces))
                {
                    unsigned int offset = 0;
                    unsigned int cnt = 0;

                    if (shader->flags & D3DVERTEX_POSITION)
                    {
                        m_meshManager.m_device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &surface->positionBuffer, &surface->size_vertex, &offset);
                        cnt++;
                    }
                    if (shader->flags & D3DVERTEX_COLOR)
                    {
                        m_meshManager.m_device->GetDeviceContext()->IASetVertexBuffers(cnt, 1, &surface->colorBuffer, &surface->size_color, &offset);
                        cnt++;
                    }
                    m_meshManager.m_device->GetDeviceContext()->IASetIndexBuffer(surface->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

                    m_meshManager.m_device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    m_meshManager.m_device->GetDeviceContext()->DrawIndexed(surface->size_listIndex, 0, 0);
                }
            }
        }
    }
}

void RenderManager::UpdateWorld()
{
    for (const auto& mesh : m_meshManager.m_meshes)
    {
        XMMATRIX mTranslation = XMMatrixTranslationFromVector(mesh->position);
        
        mesh->cb.viewMatrix = m_meshManager.m_engine->GetCam().GetCurrentCam()->cb.viewMatrix;
        mesh->cb.projectionMatrix = m_meshManager.m_engine->GetCam().GetCurrentCam()->cb.projectionMatrix;
        mesh->cb.worldMatrix = mTranslation;
    }
}