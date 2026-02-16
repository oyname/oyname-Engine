#include "gdxengine.h"
#include "RenderManager.h"
#include "Light.h"

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

void RenderManager::UpdateShadowMatrixBuffer(const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projMatrix)
{
    if (!m_objectManager.m_device)
        return;

    ID3D11Buffer* shadowMatrixBuffer = m_objectManager.m_device->GetShadowMatrixBuffer();
    if (!shadowMatrixBuffer)
        return;

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = m_objectManager.m_device->GetDeviceContext()->Map(
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

    m_objectManager.m_device->GetDeviceContext()->Unmap(shadowMatrixBuffer, 0);
}

void RenderManager::RenderShadowPass()
{
    if (!m_currentCam || !m_directionLight || !m_objectManager.m_device)
        return;

    ID3D11DeviceContext* ctx = m_objectManager.m_device->GetDeviceContext();
    ID3D11DepthStencilView* shadowDSV = m_objectManager.m_device->GetShadowMapDepthView();
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

    if (ID3D11RasterizerState* rsShadow = m_objectManager.m_device->GetShadowRasterState())
        ctx->RSSetState(rsShadow);

    UINT smW = 0, smH = 0;
    m_objectManager.m_device->GetShadowMapSize(smW, smH);
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

    if (ID3D11Buffer* shadowMatrixBuffer = m_objectManager.m_device->GetShadowMatrixBuffer())
        ctx->VSSetConstantBuffers(3, 1, &shadowMatrixBuffer);

    // ---- Draw depth into shadow map ----
    for (Shader* shader : m_objectManager.m_shaders)
    {
        if (!shader || shader->materials.empty())
            continue;

        shader->UpdateShader(m_objectManager.m_device, ShaderBindMode::VS_ONLY);

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

                mesh->Update(m_objectManager.m_device, &ms);

                for (Surface* s : mesh->surfaces)
                    if (s) s->Draw(m_objectManager.m_device, shader->flagsVertex);
            }
        }
    }
}

void RenderManager::RenderNormalPass()
{
    if (!m_currentCam || !m_objectManager.m_device)
        return;

    ID3D11DeviceContext* ctx = m_objectManager.m_device->GetDeviceContext();
    if (!ctx)
        return;

    // ---- PASS 2 STATE (deterministisch) ----
    ID3D11RenderTargetView* rtv = m_objectManager.m_device->GetRenderTargetView();
    ID3D11DepthStencilView* dsv = m_objectManager.m_device->GetDepthStencilView();
    if (!rtv || !dsv)
        return;

    ctx->OMSetRenderTargets(1, &rtv, dsv);

    if (ID3D11RasterizerState* rsDefault = m_objectManager.m_device->GetRasterizerState())
        ctx->RSSetState(rsDefault);
    else
        ctx->RSSetState(nullptr);

    // Kamera-Viewport ist Pflicht (sonst “Shadow VP” bleibt aktiv)
    ctx->RSSetViewports(1, &m_currentCam->viewport);

    // ---- Shadow resources (t7/s7) – vorbereitet für späteren echten Shadow-PS ----
    constexpr UINT SHADOW_TEX_SLOT = 7;
    ID3D11ShaderResourceView* shadowSRV = m_objectManager.m_device->GetShadowMapSRV();
    ID3D11SamplerState* shadowSampler = m_objectManager.m_device->GetComparisonSampler();

    ctx->PSSetShaderResources(SHADOW_TEX_SLOT, 1, &shadowSRV);
    ctx->PSSetSamplers(SHADOW_TEX_SLOT, 1, &shadowSampler);

    // Optional, aber konsistent: Shadow-Matrixbuffer auch im Normalpass binden
    if (m_directionLight)
    {
        Light* light = dynamic_cast<Light*>(m_directionLight);
        if (light)
        {
            UpdateShadowMatrixBuffer(light->GetLightViewMatrix(), light->GetLightProjectionMatrix());

            if (ID3D11Buffer* shadowMatrixBuffer = m_objectManager.m_device->GetShadowMatrixBuffer())
                ctx->VSSetConstantBuffers(3, 1, &shadowMatrixBuffer);
        }
    }
}

void RenderManager::RenderScene()
{
    if (!m_currentCam) {
        Debug::Log("WARNING: RenderManager::RenderScene - Camera not set");
        return;
    }

    // PASS 1: Shadow
    RenderShadowPass();

    // PASS 2: Main pass setup (OM/RS/VP + shadow bindings)
    RenderNormalPass();

    // Lichter aktualisieren
    m_lightManager.Update(m_objectManager.m_device);

    // Draw scene (nutzt jetzt garantiert Main-Pass-State)
    for (const auto& shader : m_objectManager.m_shaders)
    {
        if (!shader || shader->materials.empty())
            continue;

        shader->UpdateShader(m_objectManager.m_device);

        for (const auto& material : shader->materials)
        {
            material->SetTexture(m_objectManager.m_device);
            material->UpdateConstantBuffer(m_objectManager.m_device->GetDeviceContext());

            for (const auto& mesh : material->meshes)
            {
                MatrixSet ms = m_currentCam->matrixSet;
                ms.worldMatrix = mesh->transform.GetLocalTransformationMatrix();

                mesh->Update(m_objectManager.m_device, &ms);

                for (const auto& surface : mesh->surfaces)
                    if (surface)
                        surface->Draw(m_objectManager.m_device, shader->flagsVertex);
            }
        }
    }
}