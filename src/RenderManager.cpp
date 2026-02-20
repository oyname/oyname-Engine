#include "gdxengine.h"
#include "RenderManager.h"
#include "Light.h"

RenderManager::RenderManager(ObjectManager& objectManager, LightManager& lightManager, GDXDevice& device)
    : m_objectManager(objectManager), m_lightManager(lightManager), m_device(device),
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

void RenderManager::UpdateShadowMatrixBuffer(const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projMatrix)
{
    if (!m_device.IsInitialized())
        return;

    ID3D11Buffer* shadowMatrixBuffer = m_device.GetShadowMatrixBuffer();
    if (!shadowMatrixBuffer)
        return;

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = m_device.GetDeviceContext()->Map(
        shadowMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(hr))
        return;

    struct ShadowMatrixBuffer {
        DirectX::XMMATRIX lightViewMatrix;
        DirectX::XMMATRIX lightProjectionMatrix;
    };

    // NOTE ABOUT TRANSPOSING:
    // Your VertexShader.hlsl declares the shadow matrices as 'row_major'.
    // With 'row_major' and mul(vector, matrix) usage, you should NOT transpose here.
    // If you later remove 'row_major' in HLSL or change multiply order, flip this.
    constexpr bool HLSL_USES_ROW_MAJOR = true;

    ShadowMatrixBuffer* bufferData = (ShadowMatrixBuffer*)mappedResource.pData;
    if (HLSL_USES_ROW_MAJOR)
    {
        bufferData->lightViewMatrix = viewMatrix;
        bufferData->lightProjectionMatrix = projMatrix;
    }
    else
    {
        bufferData->lightViewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
        bufferData->lightProjectionMatrix = DirectX::XMMatrixTranspose(projMatrix);
    }

    m_device.GetDeviceContext()->Unmap(shadowMatrixBuffer, 0);
}

void RenderManager::RenderShadowPass()
{
    if (!m_currentCam || !m_directionLight || !m_device.IsInitialized())
        return;

    ID3D11DeviceContext* ctx = m_device.GetDeviceContext();
    ID3D11DepthStencilView* shadowDSV = m_device.GetShadowMapDepthView();
    if (!ctx || !shadowDSV)
        return;

    // ---- PASS 1 STATE (deterministisch) ----
    ctx->OMSetRenderTargets(0, nullptr, shadowDSV);

    // Hazard vermeiden (ShadowMap wird als SRV später gelesen)
    constexpr UINT SHADOW_TEX_SLOT = 7;
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    ctx->PSSetShaderResources(SHADOW_TEX_SLOT, 1, nullSRV);
    ctx->VSSetShaderResources(SHADOW_TEX_SLOT, 1, nullSRV);

    ctx->ClearDepthStencilView(shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

    if (ID3D11RasterizerState* rsShadow = m_device.GetShadowRasterState())
        ctx->RSSetState(rsShadow);

    UINT smW = 0, smH = 0;
    m_device.GetShadowMapSize(smW, smH);
    if (smW == 0 || smH == 0)
        return;

    D3D11_VIEWPORT vp{};
    vp.Width = (float)smW;
    vp.Height = (float)smH;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    ctx->RSSetViewports(1, &vp);

    // Depth-only
    ctx->PSSetShader(nullptr, nullptr, 0);

    // ---- Shadow Matrices updaten/binden (b3) ----
    Light* light = dynamic_cast<Light*>(m_directionLight);
    if (!light)
        return;

    const DirectX::XMMATRIX lightViewMatrix = light->GetLightViewMatrix();
    const DirectX::XMMATRIX lightProjMatrix = light->GetLightProjectionMatrix();

    UpdateShadowMatrixBuffer(lightViewMatrix, lightProjMatrix);

    if (ID3D11Buffer* shadowMatrixBuffer = m_device.GetShadowMatrixBuffer())
        ctx->VSSetConstantBuffers(3, 1, &shadowMatrixBuffer);

    // ---- Draw depth into shadow map ----
    for (Shader* shader : m_objectManager.GetShaders())
    {
        if (!shader || shader->materials.empty())
            continue;

        shader->UpdateShader(&m_device, ShaderBindMode::VS_ONLY);

        for (Material* material : shader->materials)
        {
            if (!material || material->meshes.empty())
                continue;

            // CPU-side Filter: dieses Material wirft keinen Schatten
            if (!material->castShadows)
                continue;

            for (Mesh* mesh : material->meshes)
            {
                if (!mesh) continue;

                MatrixSet ms = m_currentCam->matrixSet; // nur als Container
                ms.viewMatrix = lightViewMatrix;
                ms.projectionMatrix = lightProjMatrix;
                ms.worldMatrix = mesh->transform.GetLocalTransformationMatrix();

                mesh->Update(&m_device, &ms);

                for (Surface* s : mesh->surfaces)
                    if (s) s->Draw(&m_device, shader->flagsVertex);
            }
        }
    }
}

void RenderManager::RenderNormalPass()
{
    if (!m_currentCam || !m_device.IsInitialized())
        return;

    ID3D11DeviceContext* ctx = m_device.GetDeviceContext();
    if (!ctx)
        return;

    // ---- PASS 2 STATE (deterministisch) ----
    ID3D11RenderTargetView* rtv = m_device.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = m_device.GetDepthStencilView();
    if (!rtv || !dsv)
        return;

    ctx->OMSetRenderTargets(1, &rtv, dsv);

    if (ID3D11RasterizerState* rsDefault = m_device.GetRasterizerState())
        ctx->RSSetState(rsDefault);
    else
        ctx->RSSetState(nullptr);

    // Kamera-Viewport ist Pflicht (sonst “Shadow VP” bleibt aktiv)
    ctx->RSSetViewports(1, &m_currentCam->viewport);

    // ---- Shadow resources (t7/s7) – vorbereitet für späteren echten Shadow-PS ----
    constexpr UINT SHADOW_TEX_SLOT = 7;
    ID3D11ShaderResourceView* shadowSRV = m_device.GetShadowMapSRV();
    ID3D11SamplerState* shadowSampler = m_device.GetComparisonSampler();

    ctx->PSSetShaderResources(SHADOW_TEX_SLOT, 1, &shadowSRV);
    ctx->PSSetSamplers(SHADOW_TEX_SLOT, 1, &shadowSampler);

    // Optional, aber konsistent: Shadow-Matrixbuffer auch im Normalpass binden
    if (m_directionLight)
    {
        Light* light = dynamic_cast<Light*>(m_directionLight);
        if (light)
        {
            UpdateShadowMatrixBuffer(light->GetLightViewMatrix(), light->GetLightProjectionMatrix());

            if (ID3D11Buffer* shadowMatrixBuffer = m_device.GetShadowMatrixBuffer())
                ctx->VSSetConstantBuffers(3, 1, &shadowMatrixBuffer);
        }
    }
}

void RenderManager::RenderScene()
{
    Debug::LogOnce("RenderScene_BEGIN",
        "=== RenderScene BEGIN ===");

    if (!m_currentCam) {
        Debug::LogOnce("RenderScene_NoCamera",
            "WARNING: RenderManager::RenderScene - Camera not set");
        return;
    }

    // PASS 1
    RenderShadowPass();

    // PASS 2
    RenderNormalPass();

    // Lights
    m_lightManager.Update(&m_device);

    Debug::LogOnce("RenderScene_Camera",
        "Camera: ", Ptr(m_currentCam).c_str());

    Debug::LogOnce("RenderScene_ShaderCount",
        "Shader count: ", m_objectManager.GetShaders().size());

    for (size_t si = 0; si < m_objectManager.GetShaders().size(); ++si)
    {
        Shader* shader = m_objectManager.GetShaders()[si];

        if (!shader)
        {
            std::string key = "Shader_NULL_" + std::to_string(si);
            Debug::LogOnce(key.c_str(),
                "Shader[", si, "] = NULL");
            continue;
        }

        {
            std::string key = "Shader_" + std::to_string(si);
            Debug::LogOnce(key.c_str(),
                "Shader[", si, "]: ",
                Ptr(shader).c_str(),
                ", Materials: ",
                shader->materials.size());
        }

        if (shader->materials.empty())
            continue;

        shader->UpdateShader(&m_device);

        for (size_t mi = 0; mi < shader->materials.size(); ++mi)
        {
            Material* material = shader->materials[mi];

            if (!material)
            {
                std::string key = "Material_NULL_" + std::to_string(si) + "_" + std::to_string(mi);
                Debug::LogOnce(key.c_str(),
                    "Material[", mi, "] = NULL");
                continue;
            }

            {
                std::string key = "  Material_" + std::to_string(si) + "_" + std::to_string(mi);
                Debug::LogOnce(key.c_str(),
                    "Material[", mi, "]: ",
                    Ptr(material).c_str(),
                    ", Meshes: ",
                    material->meshes.size());
            }

            material->SetTexture(&m_device);
            material->UpdateConstantBuffer(
                m_device.GetDeviceContext());

            for (size_t mei = 0; mei < material->meshes.size(); ++mei)
            {
                Mesh* mesh = material->meshes[mei];

                if (!mesh)
                {
                    std::string key = "Mesh_NULL_" + std::to_string(si) + "_" +
                        std::to_string(mi) + "_" + std::to_string(mei);

                    Debug::LogOnce(key.c_str(),
                        " Mesh[", mei, "] = NULL");

                    continue;
                }

                {
                    std::string key =
                        "Mesh_" + std::to_string(si) + "_" +
                        std::to_string(mi) + "_" +
                        std::to_string(mei);

                    Debug::LogOnce(key.c_str(),
                        " Mesh[", mei, "]: ",
                        Ptr(mesh).c_str(),
                        ", Surfaces: ",
                        mesh->surfaces.size(),
                        ", Active: ",
                        mesh->IsActive());
                }

                mesh->Update(&m_device,
                    &m_currentCam->matrixSet);

                for (size_t sui = 0; sui < mesh->surfaces.size(); ++sui)
                {
                    Surface* surface = mesh->surfaces[sui];

                    if (!surface)
                    {
                        std::string key =
                            "Surface_NULL_" +
                            std::to_string(si) + "_" +
                            std::to_string(mi) + "_" +
                            std::to_string(mei) + "_" +
                            std::to_string(sui);

                        Debug::LogOnce(key.c_str(),
                            "   Surface[", sui, "] = NULL");

                        continue;
                    }

                    {
                        std::string key =
                            "Surface_" +
                            std::to_string(si) + "_" +
                            std::to_string(mi) + "_" +
                            std::to_string(mei) + "_" +
                            std::to_string(sui);

                        Debug::LogOnce(key.c_str(),
                            "   Surface[", sui, "]: ",
                            Ptr(surface).c_str(),
                            ", Active: ",
                            surface->isActive);
                    }

                    surface->Draw(
                        &m_device,
                        shader->flagsVertex);
                }
            }
        }
    }

    Debug::LogOnce("RenderScene_END",
        "=== RenderScene END ===");
}


//void RenderManager::RenderScene()
//{
//    if (!m_currentCam) {
//        Debug::Log("RenderManager.cpp: WARNING: RenderManager::RenderScene - Camera not set");
//        return;
//    }
//
//    // PASS 1: Shadow
//    RenderShadowPass();
//
//    // PASS 2: Main pass setup (OM/RS/VP + shadow bindings)
//    RenderNormalPass();
//
//    // Lichter aktualisieren
//    m_lightManager.Update(&m_device);
//
//    // Draw scene (nutzt jetzt garantiert Main-Pass-State)
//    for (const auto& shader : m_objectManager.GetShaders())
//    {
//        if (!shader || shader->materials.empty())
//            continue;
//
//        shader->UpdateShader(&m_device);
//
//        // m_device.GetDeviceContext()
//        for (const auto& material : shader->materials)
//        {
//            material->SetTexture(&m_device);
//            material->UpdateConstantBuffer(m_device.GetDeviceContext());
//
//            for (const auto& mesh : material->meshes)
//            {
//
//                mesh->Update(&m_device, &m_currentCam->matrixSet);
//
//                for (const auto& surface : mesh->surfaces)
//                    if (surface)
//                        surface->Draw(&m_device, shader->flagsVertex);
//            }
//        }
//    }
//}