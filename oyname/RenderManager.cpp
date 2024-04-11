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

    m_lightManager.Update(m_objectManager.m_device);

    // Iterate through all shaders and render all objects with each shader
    for (const auto& shader : m_objectManager.m_shaders)
    {
        Debug::Log("SHADER: ", shader);

        shader->UpdateShader(m_objectManager.m_device);

        for (const auto& brush : *(shader->brushes))
        {
            Debug::Log("   BRUSH: ", brush);
            // Texture and material
            for (const auto& mesh : *(brush->meshes))
            {
                Debug::Log("      MESH: ", mesh);

                // Write and set constants in the constant buffer
                mesh->UpdateConstantBuffer(m_objectManager.m_device,
                    m_currentCam->cb.viewMatrix,
                    m_currentCam->cb.projectionMatrix);

                for (const auto& surface : *(mesh->surfaces))
                {
                    Debug::Log("         SURFACE: ", surface);

                    // Render vertices
                    surface->Draw(m_objectManager.m_device, shader->flagsVertex);
                }
            }
        }
    }
}

