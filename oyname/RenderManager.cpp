#include "gdxengine.h"
#include "RenderManager.h"

RenderManager::RenderManager(ObjectManager& objectManager, LightManager& lightManager ) : m_objectManager(objectManager), m_lightManager(lightManager)
{
    m_currentCam = nullptr;
    m_directionLight = nullptr;
}

void RenderManager::SetCamera(LPENTITY camera)
{
    m_currentCam = camera;
}

void RenderManager::SetDirectionalLight(LPLIGHT dirLight)
{
    m_directionLight = dirLight;
}

void RenderManager::RenderShadow(const DirectX::XMMATRIX view, const DirectX::XMMATRIX proj)
{
    m_objectManager.m_shaders.front()->UpdateShader(m_objectManager.m_device);

    for (const auto& mesh : m_objectManager.m_meshes)
    {
        if (!mesh->surfaces->empty())
        {
            Debug::Log("      MESH: ", mesh);
            mesh->UpdateConstantBuffer(m_objectManager.m_device,
                m_currentCam->cb.viewMatrix,
                m_currentCam->cb.projectionMatrix);

            for (const auto& surface : *mesh->surfaces)
            {
                Debug::Log("         SURFACE: ", surface);
                // Render vertices
                surface->Draw(m_objectManager.m_device, D3DVERTEX_POSITION | D3DVERTEX_NORMAL);
            }
        }
    }

}

void RenderManager::RenderScene(const DirectX::XMMATRIX view, const DirectX::XMMATRIX proj)
{
    // Iterate through all shaders and render all objects with each shader
    for (const auto& shader : m_objectManager.m_shaders)
    {
        shader->UpdateShader(m_objectManager.m_device);

        for (const auto& brush : *(shader->brushes))
        {
            brush->SetTexture(m_objectManager.m_device);

            for (const auto& mesh : *(brush->meshes))
            {

                mesh->UpdateConstantBuffer(m_objectManager.m_device,view,proj);
                
                for (const auto& surface : *(mesh->surfaces))
                {
                    //Debug::Log("         SURFACE: ", surface);
                    // Render vertices
                    surface->Draw(m_objectManager.m_device, shader->flagsVertex);
                }
            }
        }
    }
}


