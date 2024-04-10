#include "gdxengine.h"
#include "RenderManager.h"

RenderManager::RenderManager(ObjectManager& objectManager, LightManager& lightManager ) : m_objectManager(objectManager), m_lightManager(lightManager)
{
    m_currentCam = nullptr;
    m_directionLight = nullptr;
}

void RenderManager::RenderLoop()
{
    RenderMesh();
}

void RenderManager::SetCamera(LPMESH camera)
{
    m_currentCam = camera;
}

void RenderManager::SetDirectionalLight(LPLIGHT dirLight)
{
    m_directionLight = dirLight;
}

void RenderManager::RenderMesh()
{
    HRESULT hr = S_OK;

    m_objectManager.m_device->ClearRenderTargetDepthStencil();

    m_directionLight->UpdateDirectionalLight(m_objectManager.m_device);

    // Geht alle Shader durch und rendert mit jedem Shader alle Objekte
    for (const auto& shader : m_objectManager.m_shaders)
    {
        Debug::Log("SHADER: ", shader);

        shader->UpdateShader(m_objectManager.m_device);

        for (const auto& brush : *(shader->brushes))
        {
            Debug::Log("   BRUSH: ", brush);
            // Textur und Material
            for (const auto& mesh : *(brush->meshes))
            {
                Debug::Log("      MESH: ", mesh);

                // Konstanten in den Constant Buffer schreiben und setzen
                mesh->UpdateConstantBuffer(m_objectManager.m_device, 
                                           m_currentCam->cb.viewMatrix, 
                                           m_currentCam->cb.projectionMatrix);

                for (const auto& surface : *(mesh->surfaces))
                {
                    Debug::Log("         SURFACE: ", surface);

                    // Vertices rendern
                    surface->Draw(m_objectManager.m_device, shader->flagsVertex);
                }
            }
        }
    }
}

// Funktion zum Aktualisieren des Richtungslichts und Kopieren der Daten in den Shader-Buffer
void RenderManager::UpdateDirectionalLight() 
{
    HRESULT hr = S_OK;

    // Aktualisieren der Position des Richtungslichts
    m_directionLight->SetPosition(m_directionLight->position);

    // Kopieren der Daten in den lightBuffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    hr = m_objectManager.m_device->GetDeviceContext()->Map(m_directionLight->lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        return;
    }

    memcpy(mappedResource.pData, &m_directionLight->cbLight, sizeof(LightSet));
    m_objectManager.m_device->GetDeviceContext()->Unmap(m_directionLight->lightBuffer, 0);

    // Setzen des lightBuffers im Shader
    m_objectManager.m_device->GetDeviceContext()->VSSetConstantBuffers(1, 1, &m_lightManager.m_lights[0]->lightBuffer);
    m_objectManager.m_device->GetDeviceContext()->PSSetConstantBuffers(1, 1, &m_lightManager.m_lights[0]->lightBuffer);
}
