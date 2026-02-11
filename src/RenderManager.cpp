#include "gdxengine.h"
#include "RenderManager.h"

RenderManager::RenderManager(ObjectManager& objectManager, LightManager& lightManager)
    : m_objectManager(objectManager), m_lightManager(lightManager),
    m_currentCam(nullptr), m_directionLight(nullptr)
{
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
    // Direkter Zugriff auf private Member (friend class erlaubt das)
    if (!m_objectManager.m_shaders.empty())
    {
        m_objectManager.m_shaders.front()->UpdateShader(m_objectManager.m_device);
    }
    for (const auto& mesh : m_objectManager.m_meshes)
    {
        if (!mesh->surfaces.empty())
        {
            for (const auto& surface : mesh->surfaces)
            {
                Debug::Log("         SURFACE: ", surface);
                surface->Draw(m_objectManager.m_device, D3DVERTEX_POSITION | D3DVERTEX_NORMAL);
            }
        }
    }
}

void RenderManager::RenderScene()
{
    // Fehlerprüfung
    if (!m_currentCam) {
        Debug::Log("WARNING: RenderManager::RenderScene - Camera not set");
        return;
    }

    // WICHTIG: Lichter ZUERST aktualisieren!
    m_lightManager.Update(m_objectManager.m_device);

    for (const auto& shader : m_objectManager.m_shaders)
    {
        // Nur setzen wenn Shader etwas zu rendern hat
        if (shader->materials.empty()) {
            continue;  // Skip leere Shader
        }

        shader->UpdateShader(m_objectManager.m_device);  // Einmal pro Shader

        for (const auto& material : shader->materials)
        {
            material->SetTexture(m_objectManager.m_device);

            // ← ADDED: Schicke Material-Daten zur GPU!
            material->UpdateConstantBuffer(m_objectManager.m_device->GetDeviceContext());

            if (material->meshes.empty()) {
                continue;  // Skip leere Materials
            }

            for (const auto& mesh : material->meshes)
            {
                MatrixSet ms = m_currentCam->matrixSet;             // view + proj von Kamera
                ms.worldMatrix = mesh->transform.GetLocalTransformationMatrix(); // world vom Mesh

                mesh->Update(m_objectManager.m_device, &ms);

                if (!mesh->surfaces.empty())
                {
                    for (const auto& surface : mesh->surfaces)
                    {
                        surface->Draw(m_objectManager.m_device, shader->flagsVertex);
                    }
                }
            }
        }
    }
}