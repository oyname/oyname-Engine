#pragma once

#include <windows.h>
#include <DirectXMath.h>

#include "gdxengine.h"


extern Timer Time;

namespace Engine
{
    extern gdx::CGIDX* engine;

    struct Color 
    {
        unsigned int r; // Rot
        unsigned int g; // Gr�n
        unsigned int b; // Blau
        unsigned int a; // Alpha (Transparenz)

        Color(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha) : r(red), g(green), b(blue), a(alpha) {}

        // �berladen des Ungleichheitsoperators !=
        bool operator!=(const Color& other) const {
            return (r != other.r || g != other.g || b != other.b || a != other.a);
        }

        bool operator==(const Color& other) const {
            return (r == other.r && g == other.g && b == other.b && a == other.a);
        }
    };

    inline unsigned int CountGfxDrivers() { return (unsigned int)engine->m_interface.interfaceManager.GetNumAdapters(); }

    inline std::string GfxDriverName() { return engine->m_interface.interfaceManager.GetGfxDriverName(engine->GetAdapterIndex()); }

    inline void SetGfxDriver(unsigned int adapter) { engine->SetAdapter(adapter); }

    inline unsigned int CountOutputs() { return (unsigned int)engine->m_interface.interfaceManager.GetCountOutput(engine->GetAdapterIndex()); }

    inline void SetOutput(unsigned int output) { engine->SetOutput(output); }

    inline unsigned int CountGfxModes(unsigned int output) { return (unsigned int)engine->m_interface.interfaceManager.GetCountDisplayModes(engine->GetAdapterIndex(),output); }

    inline unsigned int GfxModeWidth(unsigned int mode) { return engine->m_interface.interfaceManager.GetGfxModeWidth(mode, engine->GetAdapterIndex(), engine->GetOutputIndex()); }

    inline unsigned int GfxModeHeight(unsigned int mode) { return engine->m_interface.interfaceManager.GetGfxModeHeight(mode, engine->GetAdapterIndex(), engine->GetOutputIndex()); }

    inline unsigned int GetGfxModeFrequency(unsigned int mode) { return engine->m_interface.interfaceManager.GetGfxModeFrequency(mode, engine->GetAdapterIndex(), engine->GetOutputIndex()); }

    inline unsigned int GfxModeDepth() { return engine->GetColorDepth(); }

    inline bool GfxModeExists(int width, int height, int frequency) { 
        return engine->m_interface.interfaceManager.GfxModeExists(width, height, frequency, engine->GetAdapterIndex(), engine->GetOutputIndex());
    }

    inline unsigned int GfxColorDepth() { return engine->GetColorDepth(); }

    inline unsigned int GetWidth() { return engine->GetWidth(); }

    inline unsigned int GetHeight() { return engine->GetHeight(); }

    inline bool GfxFormatSupported(GXFORMAT format) { return engine->m_device.deviceManager.IsFormatSupported(engine->GetAdapterIndex(), format); }

    inline unsigned int GfxGetDirectXVersion() { return engine->m_device.deviceManager.GetDirectXVersion(engine->GetAdapterIndex()); }

    inline unsigned int GetCurrentAdapter() { return engine->GetAdapterIndex(); }

    inline int GetMaxFrequency(unsigned int width, unsigned int height){
        return engine->m_interface.interfaceManager.GetMaxFrequnecy(engine->GetAdapterIndex(), engine->GetOutputIndex(), width, height);
    }

    inline HRESULT CreateShader(LPSHADER* shader, const std::wstring& vertexShaderFile, const std::string& vertexEntryPoint, const std::wstring& pixelShaderFile, const std::string& pixelEntryPoint,DWORD flags)
    {   
        HRESULT result = S_OK;
       
       *shader = engine->GetOM().createShader();
       
       engine->GetSM().CreateShader(*shader, vertexShaderFile, vertexEntryPoint, pixelShaderFile, pixelEntryPoint);
       if (FAILED(result)) {
           return result;
       }       

       result = Engine::engine->GetILM().CreateInputLayoutVertex(&(*shader)->inputlayoutVertex, *shader, (*shader)->flagsVertex, flags);
       if (FAILED(result)) {
           return result;
       } 

       return result;
    }

    inline void CreateBrush(LPBRUSH* brush, SHADER* shader = nullptr) {
        *brush = engine->GetOM().createBrush();
        // If Shader == nullptr, then default shader."
        shader = shader == nullptr ? engine->GetSM().GetShader() : shader;
        engine->GetOM().addBrushToShader(shader, *brush);
    }
    
    inline void CreateMesh(LPENTITY *mesh, BRUSH* brush = nullptr) {
        *mesh = engine->GetOM().createMesh();
        brush = brush == nullptr ? engine->GetOM().getStandardBrush() : brush;
        engine->GetOM().addMeshToBrush(brush, *mesh);

        //(*mesh)->matrixSet.viewMatrix = engine->GetCam().GetCurrentCam()->matrixSet.viewMatrix;
        //(*mesh)->matrixSet.projectionMatrix = engine->GetCam().GetCurrentCam()->matrixSet.projectionMatrix;

        (*mesh)->GenerateViewMatrix(DirectX::XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, DirectX::XMVECTOR{ 0.0f,0.0f,1.0f,10.0f }, DirectX::XMVECTOR{ 0.0f,1.0f,0.0f,0.0f });
        (*mesh)->GenerateProjectionMatrix(XMConvertToRadians(90.0f),
            (static_cast<float>(engine->GetWidth()) / static_cast<float>(engine->GetHeight())),
            1.0f, 1000.0f);

        (*mesh)->GenerateViewport(0.0f, 0.0f, static_cast<float>(engine->GetWidth()), static_cast<float>(engine->GetHeight()), 0.0f, 1.0f);

        HRESULT hr = engine->GetBM().CreateBuffer(&((*mesh)->matrixSet), sizeof(MatrixSet), 1, D3D11_BIND_CONSTANT_BUFFER, &((*mesh)->constantBuffer));
        if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {

            return;
        }
    }

    inline void CreateLight(LPENTITY* light, D3DLIGHTTYPE type)
    {
        *light = engine->GetLM().createLight(type);
        
        (*light)->GenerateViewMatrix(DirectX::XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, DirectX::XMVECTOR{ 0.0f,0.0f,1.0f,10.0f }, DirectX::XMVECTOR{ 0.0f,1.0f,0.0f,0.0f });
        (*light)->GenerateProjectionMatrix(XMConvertToRadians(90.0f),
                                           (static_cast<float>(engine->GetWidth()) / static_cast<float>(engine->GetHeight())), 
                                           1.0f, 1000.0f);

        (*light)->GenerateViewport(0.0f, 0.0f, static_cast<float>(engine->GetWidth()), static_cast<float>(engine->GetHeight()), 0.0f, 1.0f);

        HRESULT hr = engine->GetBM().CreateBuffer(&((*light)->cbLight), sizeof(LightBufferData), 1, D3D11_BIND_CONSTANT_BUFFER, &((*light)->lightBuffer));
        if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        
            return;
        }

        hr = engine->GetBM().CreateBuffer(&((*light)->matrixSet), sizeof(MatrixSet), 1, D3D11_BIND_CONSTANT_BUFFER, &((*light)->constantBuffer));
        if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {

            return;
        }
    }

    inline void PositionEntity(LPENTITY entity, float x, float y, float z)
    {
        entity->transform.Position(x, y, z);
    }

    inline void MoveEntity(LPENTITY entity, float x, float y, float z)
    {
        entity->transform.Move(x, y, z);
    }

    inline void RotateEntity(LPENTITY entity, float fRotateX, float fRotateY, float fRotateZ, Space mode = Space::Local)
    {
        entity->transform.Rotate(fRotateX, fRotateY, fRotateZ, mode);
    }

    inline void TurnEntity(LPENTITY entity, float fRotateX, float fRotateY, float fRotateZ, Space mode = Space::Local)
    {
        entity->transform.Turn(fRotateX, fRotateY, fRotateZ, mode);
    }

    inline void CreateCamera(LPENTITY* camera)
    { 
        *camera = engine->GetOM().createMesh();
        (*camera)->GenerateViewMatrix(DirectX::XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, DirectX::XMVECTOR{ 0.0f,0.0f,1.0f,10.0f }, DirectX::XMVECTOR{ 0.0f,1.0f,0.0f,0.0f });
        (*camera)->GenerateProjectionMatrix(XMConvertToRadians(90.0f),
                                            (static_cast<float>(engine->GetWidth()) / static_cast<float>(engine->GetHeight())),
                                             1.0f, 1000.0f);
        (*camera)->GenerateViewport(0.0f, 0.0f, static_cast<float>(engine->GetWidth()), static_cast<float>(engine->GetHeight()), 0.0f, 1.0f);

        // Current Camera  
        engine->SetCamera(*camera); 
    }

    inline void CreateSurface(LPSURFACE* surface, ENTITY* lpMesh){
        *surface = engine->GetOM().createSurface();
        engine->GetOM().addSurfaceToMesh(lpMesh, *surface);
    }

    inline LPSURFACE GetSurface(LPENTITY mesh) {
        return engine->GetOM().getSurface(mesh);
    }

    inline void FillBuffer(LPSURFACE surface) {
        if (surface != nullptr && surface->pShader != nullptr)
        {
            LPSHADER shader = static_cast<LPSHADER>(surface->pShader);

            // Vertexbuffer
            if (shader->flagsVertex & D3DVERTEX_POSITION) {
                engine->GetBM().CreateBuffer(surface->position.data(), surface->size_vertex, surface->size_listVertices, D3D11_BIND_VERTEX_BUFFER, &surface->positionBuffer);
            }
            if (shader->flagsVertex & D3DVERTEX_NORMAL) {
                engine->GetBM().CreateBuffer(surface->normal.data(), surface->size_normal, surface->size_listNormal, D3D11_BIND_VERTEX_BUFFER, &surface->normalBuffer);
            }
            if (shader->flagsVertex & D3DVERTEX_COLOR) {
                engine->GetBM().CreateBuffer(surface->color.data(), surface->size_color, surface->size_listColor, D3D11_BIND_VERTEX_BUFFER, &surface->colorBuffer);
            }
            if (shader->flagsVertex & D3DVERTEX_TEX1) {
                engine->GetBM().CreateBuffer(surface->uv1.data(), surface->size_uv1, surface->size_listUV1, D3D11_BIND_VERTEX_BUFFER, &surface->uv1Buffer);
            }

            // Indexbuffer bef�llen
            engine->GetBM().CreateBuffer(surface->indices.data(), sizeof(UINT), surface->size_listIndex, D3D11_BIND_INDEX_BUFFER, &surface->indexBuffer);
        }

        surface->CalculateObjectSize();
    }

    inline void UpdateBuffer(LPSURFACE surface) {
        engine->GetBM().UpdateBuffer(surface->colorBuffer, surface->color.data(), surface->size_color);
    }

    inline void AddVertex(LPSURFACE surface, float x, float y, float z)
    {
        surface->AddVertex(x, y, z);
    }

    inline void VertexNormal(LPSURFACE surface, float x, float y, float z)
    {
        surface->VertexNormal(-1, x, y, z);
    }

    inline void VertexNormal(LPSURFACE surface, unsigned int index, float x, float y, float z)
    {
        surface->VertexNormal(index, x, y, z);
    }

    inline void VertexColor(LPSURFACE surface, unsigned int r, unsigned int g, unsigned int b)
    {
        surface->VertexColor(-1 , float(r / 255.0f), float(g / 255.0f), float(b / 255.0f));
    }

    inline void VertexColor(LPSURFACE surface, unsigned int index, unsigned int r, unsigned int g, unsigned int b)
    {
        surface->VertexColor(index, float(r / 255.0f), float(g / 255.0f), float(b / 255.0f));
    }

    inline void VertexTexCoord(LPSURFACE surface, float u, float v)
    {
        surface->VertexTexCoords(-1, u, v);
    }

    inline void AddTriangle(LPSURFACE surface, unsigned int a, unsigned int b, unsigned int c)
    {
        surface->AddIndex(a);
        surface->AddIndex(b);
        surface->AddIndex(c);
    }

    inline int Cls(int r, int g, int b, int a = 255) { return static_cast<int>(engine->Cls(float(r / 255.0f), float(g / 255.0f), float(b / 255.0f), float(a / 255.0f))); }

    inline int Flip(int syncInterval = 1) { 
        Time.update();
        return static_cast<int>(engine->m_device.Flip(syncInterval)); 
    }

    inline unsigned int Graphics(unsigned int width, unsigned int height, bool windowed = true) { return static_cast<int>(engine->Graphic(width, height, windowed)); }

    inline void RenderWorld() { 
        engine->RenderWorld(); 
    }

    inline void UpdateWorld() { 
        engine->UpdateWorld(); 
    }

    inline void LoadTexture(LPLPTEXTURE texture, const wchar_t* filename)
    {
        *texture = new TEXTURE;

        engine->GetTM().LoadTexture(engine->m_device.GetDevice(),
                                    engine->m_device.GetDeviceContext(),
                                    filename,
                                    texture);
    }

    inline void BrushTexture(LPBRUSH brush, LPTEXTURE texture)
    {
        if (brush != nullptr)
        {
            brush->SetTexture(texture->m_texture, texture->m_textureView, texture->m_imageSamplerState);
        }
    }

    inline void EntityTexture(LPENTITY entity, LPTEXTURE texture)
    {
        if (entity != nullptr && entity->pBrush != nullptr)
        {
            LPBRUSH brush = static_cast<LPBRUSH>(entity->pBrush);
        
            brush->SetTexture(texture->m_texture, texture->m_textureView, texture->m_imageSamplerState);
        }
    }

    inline void ScaleEntity(LPENTITY entity, float x, float y, float z)
    {
        entity->transform.Scale(x, y, z);
    }

    inline HRESULT CreateTexture(LPLPTEXTURE texture, int width, int height)
    {
        *texture = new TEXTURE;
        return (*texture)->CreateTexture(engine->m_device.GetDevice(), width, height);
    }

    inline HRESULT LockBuffer(LPTEXTURE texture)
    {
        return texture->LockBuffer(engine->m_device.GetDeviceContext());
    }

    inline void UnlockBuffer(LPTEXTURE texture)
    {
        texture->UnlockBuffer(engine->m_device.GetDeviceContext());
    }

    inline void SetPixel(LPTEXTURE texture, int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char alpha)
    {
        texture->SetPixel(engine->m_device.GetDeviceContext(), x, y, r, g, b, alpha);
    }

    inline Color GetColor(LPTEXTURE texture, int x, int y)
    {
        unsigned char r, g, b, alpha;

        texture->GetPixel(x, y, r, g, b, alpha);

        return Color(r, g, b, alpha);
    }

    inline void SetCamera(LPENTITY mesh)
    {
        engine->SetCamera(mesh);
    }

    inline void LookAt(LPENTITY entity, float targetX, float targetY, float targetZ)
    {
        entity->transform.LookAt(DirectX::XMVectorSet(targetX, targetY, targetZ, 0.0f), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    }

    inline void RotateTowardsVector(LPENTITY entity, float targetX, float targetY, float targetZ)
    {
        entity->transform.RotateTowardsVector(DirectX::XMVectorSet(targetX, targetY, targetZ, 0.0f), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    }

} // End of namespace Engine
