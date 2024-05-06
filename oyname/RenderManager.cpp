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

void RenderManager::SetDirectionalLight(LPENTITY dirLight)
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
            for (const auto& surface : *mesh->surfaces)
            {
                Debug::Log("         SURFACE: ", surface);
                // Render vertices
                surface->Draw(m_objectManager.m_device, D3DVERTEX_POSITION | D3DVERTEX_NORMAL);
            }
        }
    }

}

void RenderManager::RenderScene()
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
                mesh->Update(m_objectManager.m_device, &this->m_currentCam->matrixSet);
                
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


