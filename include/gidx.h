#pragma once

#include <windows.h>
#include <DirectXMath.h>
#include <fstream>  

#include "gdxengine.h"

extern Timer Time;

namespace Engine
{
    extern gdx::CGIDX* engine;

    // VSync-Modus
    enum class VSync {
        OFF = 0,
        ON = 1
    };
    static Engine::VSync vSyncMode = Engine::VSync::ON;

    struct Color
    {
        unsigned int r; // Rot
        unsigned int g; // Grün
        unsigned int b; // Blau
        unsigned int a; // Alpha (Transparenz)

        Color(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
            : r(red), g(green), b(blue), a(alpha) {
        }

        // Überladen des Ungleichheitsoperators !=
        bool operator!=(const Color& other) const {
            return (r != other.r || g != other.g || b != other.b || a != other.a);
        }

        bool operator==(const Color& other) const {
            return (r == other.r && g == other.g && b == other.b && a == other.a);
        }
    };

    // ==================== GRAPHICS INFO ====================

    inline unsigned int CountGfxDrivers() {
        return (unsigned int)engine->m_interface.interfaceManager.GetNumAdapters();
    }

    inline std::string GfxDriverName() {
        return engine->m_interface.interfaceManager.GetGfxDriverName(engine->GetAdapterIndex());
    }

    inline void SetGfxDriver(unsigned int adapter) {
        engine->SetAdapter(adapter);
    }

    inline unsigned int CountOutputs() {
        return (unsigned int)engine->m_interface.interfaceManager.GetCountOutput(engine->GetAdapterIndex());
    }

    inline void SetOutput(unsigned int output) {
        engine->SetOutput(output);
    }

    inline unsigned int CountGfxModes(unsigned int output) {
        return (unsigned int)engine->m_interface.interfaceManager.GetCountDisplayModes(engine->GetAdapterIndex(), output);
    }

    inline unsigned int GfxModeWidth(unsigned int mode) {
        return engine->m_interface.interfaceManager.GetGfxModeWidth(mode, engine->GetAdapterIndex(), engine->GetOutputIndex());
    }

    inline unsigned int GfxModeHeight(unsigned int mode) {
        return engine->m_interface.interfaceManager.GetGfxModeHeight(mode, engine->GetAdapterIndex(), engine->GetOutputIndex());
    }

    inline unsigned int GetGfxModeFrequency(unsigned int mode) {
        return engine->m_interface.interfaceManager.GetGfxModeFrequency(mode, engine->GetAdapterIndex(), engine->GetOutputIndex());
    }

    inline unsigned int GfxModeDepth() {
        return engine->GetColorDepth();
    }

    inline bool GfxModeExists(int width, int height, int frequency) {
        return engine->m_interface.interfaceManager.GfxModeExists(width, height, frequency, engine->GetAdapterIndex(), engine->GetOutputIndex());
    }

    inline unsigned int GfxColorDepth() {
        return engine->GetColorDepth();
    }

    inline unsigned int GetWidth() {
        return engine->GetWidth();
    }

    inline unsigned int GetHeight() {
        return engine->GetHeight();
    }

    inline bool GfxFormatSupported(GXFORMAT format) {
        return engine->m_device.deviceManager.IsFormatSupported(engine->GetAdapterIndex(), format);
    }

    inline unsigned int GfxGetDirectXVersion() {
        return engine->m_device.deviceManager.GetDirectXVersion(engine->GetAdapterIndex());
    }

    inline unsigned int GetCurrentAdapter() {
        return engine->GetAdapterIndex();
    }

    inline int GetMaxFrequency(unsigned int width, unsigned int height) {
        return engine->m_interface.interfaceManager.GetMaxFrequnecy(engine->GetAdapterIndex(), engine->GetOutputIndex(), width, height);
    }

    // Diese Funktionen arbeiten mit allen Entity-Typen (Mesh, Camera, Light)
    inline void PositionEntity(LPENTITY entity, float x, float y, float z)
    {
        if (entity == nullptr) {
            Debug::Log("ERROR: PositionEntity - entity is nullptr");
            return;
        }
        entity->transform.Position(x, y, z);
    }

    inline void PositionEntity(LPENTITY entity, DirectX::XMVECTOR pos)
    {
        if (entity == nullptr) {
            Debug::Log("ERROR: PositionEntity - entity is nullptr");
            return;
        }
        entity->transform.SetPosition(pos);
    }

    inline void MoveEntity(LPENTITY entity, float x, float y, float z, Space mode = Space::Local)
    {
        if (entity == nullptr) {
            Debug::Log("ERROR: MoveEntity - entity is nullptr");
            return;
        }
        entity->transform.Move(x, y, z, mode);
    }

    inline void RotateEntity(LPENTITY entity, float fRotateX, float fRotateY, float fRotateZ, Space mode = Space::Local)
    {
        if (entity == nullptr) {
            Debug::Log("ERROR: RotateEntity - entity is nullptr");
            return;
        }
        entity->transform.Rotate(fRotateX, fRotateY, fRotateZ, mode);
    }

    inline void RotateEntity(LPENTITY entity, DirectX::XMVECTOR quaternion)
    {
        if (entity == nullptr) {
            Debug::Log("ERROR: RotateEntity - entity is nullptr");
            return;
        }
        entity->transform.SetRotationQuaternion(quaternion);
    }

    inline void TurnEntity(LPENTITY entity, float fRotateX, float fRotateY, float fRotateZ, Space mode = Space::Local)
    {
        if (entity == nullptr) {
            Debug::Log("ERROR: TurnEntity - entity is nullptr");
            return;
        }
        entity->transform.Turn(fRotateX, fRotateY, fRotateZ, mode);
    }

    inline void ScaleEntity(LPENTITY entity, float x, float y, float z)
    {
        if (entity == nullptr) {
            Debug::Log("ERROR: ScaleEntity - entity is nullptr");
            return;
        }
        entity->transform.Scale(x, y, z);
    }

    inline void LookAt(LPENTITY entity, float targetX, float targetY, float targetZ)
    {
        if (entity == nullptr) {
            Debug::Log("ERROR: LookAt - entity is nullptr");
            return;
        }
        DirectX::XMVECTOR target = DirectX::XMVectorSet(targetX, targetY, targetZ, 1.0f);
        DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        entity->transform.LookAt(target, up);
    }

    inline void LookAt(LPENTITY entity, DirectX::XMVECTOR target)
    {
        if (entity == nullptr) {
            Debug::Log("ERROR: LookAt - entity is nullptr");
            return;
        }
        DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        entity->transform.LookAt(target, up);
    }

    // ==================== CAMERA ====================

    inline void CreateCamera(LPENTITY* camera)
    {
        if (camera == nullptr) {
            Debug::Log("ERROR: CreateCamera - camera pointer is nullptr");
            return;
        }

        Camera* cam = engine->GetOM().createCamera();
        if (cam == nullptr) {
            Debug::Log("ERROR: CreateCamera - Failed to create camera");
            return;
        }

        // Initiale View-Matrix: Kamera bei (0,0,0) schaut nach +Z
        cam->GenerateViewMatrix(
            DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),     // Position
            DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f),     // Zielpunkt (nach vorne)
            DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)      // Up
        );

        cam->GenerateProjectionMatrix(
            XMConvertToRadians(60.0f),
            (static_cast<float>(engine->GetWidth()) / static_cast<float>(engine->GetHeight())),
            0.1f, 1000.0f
        );

        cam->GenerateViewport(
            0.0f, 0.0f,
            static_cast<float>(engine->GetWidth()),
            static_cast<float>(engine->GetHeight()),
            0.0f, 1.0f
        );

        *camera = cam;
        engine->SetCamera(cam);
    }

    inline void SetCamera(LPENTITY camera)
    {
        if (camera == nullptr) {
            Debug::Log("ERROR: SetCamera - camera is nullptr");
            return;
        }
        engine->SetCamera(camera);  // engine->SetCamera macht den Cast intern
    }

    inline void PositionLightAtCamera(Light* light, class Camera* camera,
        DirectX::XMVECTOR offset = DirectX::XMVectorZero())
    {
        engine->GetLM().PositionLightAtCamera(light, camera, offset);
        engine->GetLM().Update(&engine->m_device);  // GPU-Buffer aktualisieren
    }

    // ==================== LIGHT ====================

    inline void CreateLight(LPENTITY* light, D3DLIGHTTYPE type)
    {
        if (light == nullptr) {
            Debug::Log("ERROR: CreateLight - light pointer is nullptr");
            return;
        }

        // ← GEÄNDERT: Nutze LightManager statt ObjectManager
        Light* l = engine->GetLM().createLight(type);
        if (l == nullptr) {
            Debug::Log("ERROR: CreateLight - Failed to create light");
            return;
        }

        // View/Projection für Shadow-Mapping
        l->GenerateViewMatrix(
            DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
            DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 10.0f),
            DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
        );

        l->GenerateProjectionMatrix(
            XMConvertToRadians(90.0f),
            (static_cast<float>(engine->GetWidth()) / static_cast<float>(engine->GetHeight())),
            1.0f, 1000.0f
        );

        l->GenerateViewport(
            0.0f, 0.0f,
            static_cast<float>(engine->GetWidth()),
            static_cast<float>(engine->GetHeight()),
            0.0f, 1.0f
        );

        // Light Buffer erstellen
        HRESULT hr = engine->GetBM().CreateBuffer(
            &l->cbLight,
            sizeof(LightBufferData),
            1,
            D3D11_BIND_CONSTANT_BUFFER,
            &l->lightBuffer
        );

        if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
            Debug::Log("ERROR: CreateLight - Failed to create light buffer");
            return;
        }

        // Constant Buffer für Matrizen
        hr = engine->GetBM().CreateBuffer(
            &l->matrixSet,
            sizeof(MatrixSet),
            1,
            D3D11_BIND_CONSTANT_BUFFER,
            &l->constantBuffer
        );
        if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
            Debug::Log("ERROR: CreateLight - Failed to create constant buffer");
            return;
        }

        *light = l;
    }

    // Globale Ambient-Farbe (gilt für die ganze Szene)
    inline void SetAmbientColor(float r, float g, float b, float a = 1.0f)
    {
        engine->SetGlobalAmbient(DirectX::XMFLOAT4(r, g, b, a));
    }

    inline void LightColor(LPENTITY light, float r, float g, float b, float a = 1.0f)
    {
        if (light == nullptr) {
            Debug::Log("ERROR: LightColor - light is nullptr");
            return;
        }

        Light* l = dynamic_cast<Light*>(light);
        if (l == nullptr) {
            Debug::Log("ERROR: LightColor - Entity is not a Light!");
            return;
        }

        l->SetDiffuseColor(DirectX::XMFLOAT4(r, g, b, a));
    }

    inline void CreateMesh(LPENTITY* mesh, MATERIAL* material = nullptr)
    {
        if (mesh == nullptr) {
            Debug::Log("ERROR: CreateMesh - mesh pointer is nullptr");
            return;
        }

        Mesh* m = engine->GetOM().createMesh();
        if (m == nullptr) {
            Debug::Log("ERROR: CreateMesh - Failed to create mesh");
            return;
        }

        material = material == nullptr ? engine->GetOM().getStandardMaterial() : material;

        if (material == nullptr) {
            Debug::Log("ERROR: CreateMesh - No valid material available");
            engine->GetOM().deleteMesh(m);
            return;
        }

        engine->GetOM().addMeshToMaterial(material, m);

        HRESULT hr = engine->GetBM().CreateBuffer(
            &m->matrixSet,
            sizeof(MatrixSet),
            1,
            D3D11_BIND_CONSTANT_BUFFER,
            &m->constantBuffer
        );
        if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
            Debug::Log("ERROR: CreateMesh - Failed to create constant buffer");
            engine->GetOM().deleteMesh(m);
            return;
        }

        *mesh = m;
    }

    // ==================== SHADER ====================

    inline HRESULT CreateShader(LPSHADER* shader,
        const std::wstring& vertexShaderFile,
        const std::string& vertexEntryPoint,
        const std::wstring& pixelShaderFile,
        const std::string& pixelEntryPoint,
        DWORD flags)
    {
        HRESULT result = S_OK;

        // 1. Validiere Input-Parameter
        if (shader == nullptr) {
            Debug::Log("ERROR: Engine::CreateShader - shader pointer is nullptr");
            return E_INVALIDARG;
        }

        if (vertexShaderFile.empty() || pixelShaderFile.empty()) {
            Debug::Log("ERROR: Engine::CreateShader - shader file paths are empty");
            return E_INVALIDARG;
        }

        if (engine == nullptr) {
            Debug::Log("ERROR: Engine::CreateShader - engine is nullptr");
            return E_FAIL;
        }

        // 2. Erstelle Shader-Objekt
        *shader = engine->GetOM().createShader();
        if (*shader == nullptr) {
            Debug::Log("ERROR: Engine::CreateShader - Failed to create shader object");
            return E_OUTOFMEMORY;
        }

        // 3. Compiliere Vertex und Pixel Shader
        result = engine->GetSM().CreateShader(*shader,
            vertexShaderFile,
            vertexEntryPoint,
            pixelShaderFile,
            pixelEntryPoint);
        if (FAILED(result)) {
            Debug::Log("ERROR: Engine::CreateShader - Shader compilation failed");
            Debug::Log("  Vertex Shader: ", vertexShaderFile.c_str());
            Debug::Log("  Pixel Shader: ", pixelShaderFile.c_str());
            Debug::GetErrorMessage(__FILE__, __LINE__, result);

            engine->GetOM().deleteShader(*shader);
            return result;
        }

        // 4. Setze Vertex Format Flags
        (*shader)->flagsVertex = flags;

        // 5. Erstelle Input Layout
        result = engine->GetILM().CreateInputLayoutVertex(
            &(*shader)->inputlayoutVertex,
            *shader,
            (*shader)->flagsVertex,
            flags
        );
        if (FAILED(result)) {
            Debug::Log("ERROR: Engine::CreateShader - Input layout creation failed");
            Debug::GetErrorMessage(__FILE__, __LINE__, result);

            if ((*shader)->vertexShader) (*shader)->vertexShader->Release();
            if ((*shader)->pixelShader) (*shader)->pixelShader->Release();
            engine->GetOM().deleteShader(*shader);
            return result;
        }

        // 6. Freigeben der Blobs nach Input Layout Creation
        if ((*shader)->blobVS != nullptr) {
            (*shader)->blobVS->Release();
            (*shader)->blobVS = nullptr;
        }
        if ((*shader)->blobPS != nullptr) {
            (*shader)->blobPS->Release();
            (*shader)->blobPS = nullptr;
        }

        return S_OK;
    }

    inline DWORD CreateVertexFlags(
        bool hasPosition = true,
        bool hasNormal = false,
        bool hasColor = false,
        bool hasTexCoord1 = false,
        bool hasTexCoord2 = false)
    {
        DWORD flags = 0;
        if (hasPosition)   flags |= D3DVERTEX_POSITION;
        if (hasNormal)     flags |= D3DVERTEX_NORMAL;
        if (hasColor)      flags |= D3DVERTEX_COLOR;
        if (hasTexCoord1)  flags |= D3DVERTEX_TEX1;
        if (hasTexCoord2)  flags |= D3DVERTEX_TEX2;
        return flags;
    }

    // ==================== MATERIAL ====================

    inline void CreateMaterial(LPMATERIAL* material, SHADER* shader = nullptr)
    {
        if (material == nullptr) {
            Debug::Log("ERROR: CreateMaterial - material pointer is nullptr");
            return;
        }

        *material = engine->GetOM().createMaterial();
        if (*material == nullptr) {
            Debug::Log("ERROR: CreateMaterial - Failed to create material");
            return;
        }

        // If Shader == nullptr, then default shader
        shader = shader == nullptr ? engine->GetSM().GetShader() : shader;

        if (shader == nullptr) {
            Debug::Log("ERROR: CreateMaterial - No valid shader available");
            engine->GetOM().deleteMaterial(*material);
            *material = nullptr;
            return;
        }

        engine->GetOM().assignShaderToMaterial(shader, *material);
    }

    inline void CreateSurface(LPSURFACE* surface, LPENTITY entity)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: CreateSurface - surface pointer is nullptr");
            return;
        }

        if (entity == nullptr) {
            Debug::Log("ERROR: CreateSurface - entity is nullptr");
            return;
        }

        *surface = engine->GetOM().createSurface();
        if (*surface == nullptr) {
            Debug::Log("ERROR: CreateSurface - Failed to create surface");
            return;
        }

        // Prüfe ob Entity ein Mesh ist (nur Meshes haben Surfaces)
        Mesh* mesh = dynamic_cast<Mesh*>(entity);
        if (mesh == nullptr) {
            Debug::Log("ERROR: CreateSurface - Entity is not a Mesh!");
            engine->GetOM().deleteSurface(*surface);
            *surface = nullptr;
            return;
        }

        engine->GetOM().addSurfaceToMesh(mesh, *surface);
    }

    inline LPSURFACE GetSurface(LPENTITY entity)
    {
        if (entity == nullptr) {
            Debug::Log("ERROR: GetSurface - entity is nullptr");
            return nullptr;
        }

        Mesh* mesh = dynamic_cast<Mesh*>(entity);
        if (mesh == nullptr) {
            Debug::Log("ERROR: GetSurface - Entity is not a Mesh!");
            return nullptr;
        }

        return engine->GetOM().getSurface(mesh);
    }

    inline void FillBuffer(LPSURFACE surface)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: FillBuffer - surface is nullptr");
            return;
        }

        if (surface->pShader == nullptr) {
            Debug::Log("ERROR: FillBuffer - surface has no shader");
            return;
        }

        LPSHADER shader = static_cast<LPSHADER>(surface->pShader);

        // Vertexbuffer
        if (shader->flagsVertex & D3DVERTEX_POSITION) {
            engine->GetBM().CreateBuffer(surface->position.data(), surface->size_position,
                surface->size_listPosition, D3D11_BIND_VERTEX_BUFFER,
                &surface->positionBuffer);
        }
        if (shader->flagsVertex & D3DVERTEX_NORMAL) {
            engine->GetBM().CreateBuffer(surface->normal.data(), surface->size_normal,
                surface->size_listNormal, D3D11_BIND_VERTEX_BUFFER,
                &surface->normalBuffer);
        }
        if (shader->flagsVertex & D3DVERTEX_COLOR) {
            engine->GetBM().CreateBuffer(surface->color.data(), surface->size_color,
                surface->size_listColor, D3D11_BIND_VERTEX_BUFFER,
                &surface->colorBuffer);
        }
        if (shader->flagsVertex & D3DVERTEX_TEX1) {
            engine->GetBM().CreateBuffer(surface->uv1.data(), surface->size_uv1,
                surface->size_listUV1, D3D11_BIND_VERTEX_BUFFER,
                &surface->uv1Buffer);
        }

        // Indexbuffer befüllen
        engine->GetBM().CreateBuffer(surface->indices.data(), sizeof(UINT),
            surface->size_listIndex, D3D11_BIND_INDEX_BUFFER,
            &surface->indexBuffer);
    }

    inline void UpdateColorBuffer(LPSURFACE surface)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: UpdateColorBuffer - surface is nullptr");
            return;
        }
        engine->GetBM().UpdateBuffer(surface->colorBuffer, surface->color.data(), surface->size_color);
    }

    inline void UpdateVertexBuffer(LPSURFACE surface)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: UpdateVertexBuffer - surface is nullptr");
            return;
        }
        engine->GetBM().UpdateBuffer(surface->positionBuffer, surface->position.data(),
            surface->size_position * surface->size_listPosition);
    }

    inline void AddVertex(LPSURFACE surface, float x, float y, float z)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: AddVertex - surface is nullptr");
            return;
        }
        surface->AddVertex(-1, x, y, z);
    }

    inline void AddVertex(int index, LPSURFACE surface, DirectX::XMVECTOR vec)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: AddVertex - surface is nullptr");
            return;
        }
        surface->AddVertex(index, DirectX::XMVectorGetX(vec), DirectX::XMVectorGetY(vec), DirectX::XMVectorGetZ(vec));
    }

    inline void AddVertex(LPSURFACE surface, DirectX::XMVECTOR vec)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: AddVertex - surface is nullptr");
            return;
        }
        surface->AddVertex(-1, DirectX::XMVectorGetX(vec), DirectX::XMVectorGetY(vec), DirectX::XMVectorGetZ(vec));
    }

    inline void AddVertex(LPSURFACE surface, DirectX::XMFLOAT3 vec)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: AddVertex - surface is nullptr");
            return;
        }
        surface->AddVertex(-1, vec.x, vec.y, vec.z);
    }

    inline void AddVertex(int index, LPSURFACE surface, DirectX::XMFLOAT3 vec)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: AddVertex - surface is nullptr");
            return;
        }
        surface->AddVertex(index, vec.x, vec.y, vec.z);
    }

    inline void VertexNormal(LPSURFACE surface, float x, float y, float z)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: VertexNormal - surface is nullptr");
            return;
        }
        surface->VertexNormal(-1, x, y, z);
    }

    inline void VertexNormal(LPSURFACE surface, unsigned int index, float x, float y, float z)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: VertexNormal - surface is nullptr");
            return;
        }
        surface->VertexNormal(index, x, y, z);
    }

    inline void VertexColor(LPSURFACE surface, unsigned int r, unsigned int g, unsigned int b)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: VertexColor - surface is nullptr");
            return;
        }
        surface->VertexColor(-1, float(r / 255.0f), float(g / 255.0f), float(b / 255.0f));
    }

    inline void VertexColor(LPSURFACE surface, unsigned int index, unsigned int r, unsigned int g, unsigned int b)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: VertexColor - surface is nullptr");
            return;
        }
        surface->VertexColor(index, float(r / 255.0f), float(g / 255.0f), float(b / 255.0f));
    }

    inline void VertexTexCoord(LPSURFACE surface, float u, float v)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: VertexTexCoord - surface is nullptr");
            return;
        }
        surface->VertexTexCoords(-1, u, v);
    }

    inline void AddTriangle(LPSURFACE surface, unsigned int a, unsigned int b, unsigned int c)
    {
        if (surface == nullptr) {
            Debug::Log("ERROR: AddTriangle - surface is nullptr");
            return;
        }
        surface->AddIndex(a);
        surface->AddIndex(b);
        surface->AddIndex(c);
    }

    // ==================== RENDERING ====================

    inline int Cls(int r, int g, int b, int a = 255)
    {
        return static_cast<int>(engine->Cls(float(r / 255.0f), float(g / 255.0f), float(b / 255.0f), float(a / 255.0f)));
    }

    inline int Flip()
    {
        return static_cast<int>(engine->m_device.Flip(static_cast<int>(vSyncMode)));
    }

    inline void SetVSync(VSync mode)
    {
        vSyncMode = mode;
    }

    inline Engine::VSync GetVSync()
    {
        return vSyncMode;
    }

    inline unsigned int Graphics(unsigned int width, unsigned int height, bool windowed = true)
    {
        return static_cast<int>(engine->Graphic(width, height, windowed));
    }

    inline HRESULT RenderWorld()
    {
        return engine->RenderWorld();
    }

    inline void UpdateWorld()
    {
        engine->UpdateWorld();
    }

    // ==================== TEXTURE ====================

    inline void LoadTexture(LPLPTEXTURE texture, const wchar_t* filename)
    {

        // Prüfe ob Datei existiert
        std::wifstream file(filename);
        if (!file.good()) {
            Debug::Log("ERROR: File not found!");
            return;
        }

        *texture = new TEXTURE;

        HRESULT hr = engine->GetTM().LoadTexture(
            engine->m_device.GetDevice(),
            engine->m_device.GetDeviceContext(),
            filename,
            texture
        );

        if (FAILED(hr)) {
            Debug::Log("ERROR: Failed to load texture");
            Debug::GetErrorMessage(__FILE__, __LINE__, hr);
        }
    }

    inline void MaterialTexture(LPMATERIAL material, LPTEXTURE texture)
    {
        if (material == nullptr) {
            Debug::Log("ERROR: MaterialTexture - material is nullptr");
            return;
        }

        if (texture == nullptr) {
            Debug::Log("ERROR: MaterialTexture - texture is nullptr");
            return;
        }

        material->SetTexture(texture->m_texture, texture->m_textureView, texture->m_imageSamplerState);
        material->UpdateConstantBuffer(engine->m_device.GetDeviceContext());
    }

    inline void EntityTexture(LPENTITY entity, LPTEXTURE texture)
    {
        if (entity == nullptr) {
            Debug::Log("ERROR: EntityTexture - entity is nullptr");
            return;
        }

        if (texture == nullptr) {
            Debug::Log("ERROR: EntityTexture - texture is nullptr");
            return;
        }

        // Prüfe ob Entity ein Mesh ist
        Mesh* mesh = dynamic_cast<Mesh*>(entity);
        if (mesh == nullptr) {
            Debug::Log("ERROR: EntityTexture - Entity is not a Mesh!");
            return;
        }

        if (mesh->pMaterial != nullptr)
        {
            LPMATERIAL material = static_cast<LPMATERIAL>(mesh->pMaterial);
            material->SetTexture(texture->m_texture, texture->m_textureView, texture->m_imageSamplerState);
        }
        else {
            Debug::Log("ERROR: EntityTexture - Mesh has no material");
        }
    }

    inline HRESULT CreateTexture(LPLPTEXTURE texture, int width, int height)
    {
        if (texture == nullptr) {
            Debug::Log("ERROR: CreateTexture - texture pointer is nullptr");
            return E_INVALIDARG;
        }

        *texture = new TEXTURE;
        if (*texture == nullptr) {
            Debug::Log("ERROR: CreateTexture - Failed to allocate texture");
            return E_OUTOFMEMORY;
        }

        return (*texture)->CreateTexture(engine->m_device.GetDevice(), width, height);
    }

    inline HRESULT LockBuffer(LPTEXTURE texture)
    {
        if (texture == nullptr) {
            Debug::Log("ERROR: LockBuffer - texture is nullptr");
            return E_INVALIDARG;
        }
        return texture->LockBuffer(engine->m_device.GetDeviceContext());
    }

    inline void UnlockBuffer(LPTEXTURE texture)
    {
        if (texture == nullptr) {
            Debug::Log("ERROR: UnlockBuffer - texture is nullptr");
            return;
        }
        texture->UnlockBuffer(engine->m_device.GetDeviceContext());
    }

    inline void SetPixel(LPTEXTURE texture, int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char alpha)
    {
        if (texture == nullptr) {
            Debug::Log("ERROR: SetPixel - texture is nullptr");
            return;
        }
        texture->SetPixel(engine->m_device.GetDeviceContext(), x, y, r, g, b, alpha);
    }

    inline Color GetColor(LPTEXTURE texture, int x, int y)
    {
        if (texture == nullptr) {
            Debug::Log("ERROR: GetColor - texture is nullptr");
            return Color(0, 0, 0, 0);
        }

        unsigned char r, g, b, alpha;
        texture->GetPixel(x, y, r, g, b, alpha);

        return Color(r, g, b, alpha);
    }

    // ==================== MESH-SPECIFIC FUNCTIONS ====================
    // Diese Funktionen funktionieren nur mit Meshes

    inline void EntityCollisionMode(LPENTITY entity, COLLISION mode)
    {
        if (entity == nullptr) {
            Debug::Log("ERROR: EntityCollisionMode - entity is nullptr");
            return;
        }

        Mesh* mesh = dynamic_cast<Mesh*>(entity);
        if (mesh == nullptr) {
            Debug::Log("ERROR: EntityCollisionMode - Entity is not a Mesh!");
            return;
        }

        mesh->SetCollisionMode(mode);
    }

    inline LPSURFACE EntitySurface(LPENTITY entity, unsigned int index = 0)
    {
        if (entity == nullptr) {
            Debug::Log("ERROR: EntitySurface - entity is nullptr");
            return nullptr;
        }

        Mesh* mesh = dynamic_cast<Mesh*>(entity);
        if (mesh == nullptr) {
            Debug::Log("ERROR: EntitySurface - Entity is not a Mesh!");
            return nullptr;
        }

        return mesh->GetSurface(index);
    }

    inline bool EntityCollision(LPENTITY entity1, LPENTITY entity2)
    {
        if (entity1 == nullptr || entity2 == nullptr) {
            Debug::Log("ERROR: EntityCollision - one or both entities are nullptr");
            return false;
        }

        Mesh* mesh1 = dynamic_cast<Mesh*>(entity1);
        Mesh* mesh2 = dynamic_cast<Mesh*>(entity2);

        if (mesh1 == nullptr || mesh2 == nullptr) {
            Debug::Log("ERROR: EntityCollision - one or both entities are not Meshes!");
            return false;
        }

        return mesh1->CheckCollision(mesh2);
    }

    inline DirectX::BoundingOrientedBox* EntityOBB(LPENTITY entity)
    {
        if (entity == nullptr) {
            Debug::Log("ERROR: EntityOBB - entity is nullptr");
            return nullptr;
        }

        Mesh* mesh = dynamic_cast<Mesh*>(entity);
        if (mesh == nullptr) {
            Debug::Log("ERROR: EntityOBB - Entity is not a Mesh!");
            return nullptr;
        }

        return &mesh->obb;
    }
} // End of namespace Engine