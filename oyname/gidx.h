#pragma once

#include <windows.h>
#include <DirectXMath.h>

#include "gdxengine.h"


namespace Engine
{
    extern gdx::CGIDX* engine;

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

    inline HRESULT CreateShader(LPSHADER* shader, const std::wstring& vertexShaderFile, const std::wstring& pixelShaderFile, const std::string& entryPoint,DWORD flags)
    {   
        HRESULT result = S_OK;
       
       *shader = engine->GetOM().createShader();
       
       engine->GetSM().CreateShader(*shader, vertexShaderFile, pixelShaderFile, entryPoint);
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
    
    inline void CreateMesh(LPMESH *mesh, BRUSH* brush = nullptr) {
        *mesh = engine->GetOM().createMesh();
        brush = brush == nullptr ? engine->GetOM().getStandardBrush() : brush;
        engine->GetOM().addMeshToBrush(brush, *mesh);

        (*mesh)->mTranslation = XMMatrixTranslationFromVector((*mesh)->position);
        (*mesh)->cb.viewMatrix = engine->GetCam().GetCurrentCam()->cb.viewMatrix;
        (*mesh)->cb.projectionMatrix = engine->GetCam().GetCurrentCam()->cb.projectionMatrix;

        HRESULT hr = engine->GetBM().CreateBuffer(&((*mesh)->cb), sizeof(MatrixSet), 1, D3D11_BIND_CONSTANT_BUFFER, &((*mesh)->constantBuffer));
        if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {

            return;
        }
    }

    inline void CreateLight(LPLIGHT* light, D3DLIGHTTYPE type)
    {
        *light = engine->GetLM().createLight(type);
        
        if(type == D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL)
            engine->SetDirectionalLight(*light);

        HRESULT hr = engine->GetBM().CreateBuffer(&((*light)->cbLight), sizeof(LightSet), 1, D3D11_BIND_CONSTANT_BUFFER, &((*light)->lightBuffer));
        if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
        
            return;
        }
    }

    inline void PositionEntity(LPMESH mesh, float x, float y, float z)
    {
        mesh->PositionEntity(x, y, z);
    }

    inline void MoveEntity(LPMESH mesh, float x, float y, float z)
    {
        mesh->MoveEntity(x, y, z);
    }

    inline void RotateEntity(LPMESH mesh, float fRotateX, float fRotateY, float fRotateZ)
    {
        mesh->RotateEntity(fRotateX, fRotateY, fRotateZ);
    }

    inline void TurnEntity(LPMESH mesh, float fRotateX, float fRotateY, float fRotateZ, Space mode = Space::Local)
    {
        mesh->TurnEntity(fRotateX, fRotateY, fRotateZ, mode);
    }

    inline void CreateCamera(LPMESH* camera)
    {
        engine->GetCam().SetPerspective(XMConvertToRadians(75.0f),
                                        (static_cast<float>(engine->GetWidth()) / static_cast<float>(engine->GetHeight())),
                                        1.0f,
                                        1000.0f);

        engine->GetCam().CreateCamera(camera); 

        // Current Camera  
        engine->SetCamera(*camera); 
    }

    inline void CreateSurface(LPSURFACE* surface, MESH* lpMesh){
        *surface = engine->GetOM().createSurface();
        engine->GetOM().addSurfaceToMesh(lpMesh, *surface);
    }

    inline LPSURFACE GetSurface(LPMESH mesh) {
        return engine->GetOM().getSurface(mesh);
    }

    inline void FillBuffer(LPSURFACE surface) {
        // Vertexbuffer
        if (surface->pShader->flagsVertex & D3DVERTEX_POSITION) {
            engine->GetBM().CreateBuffer(surface->position.data(), surface->size_vertex, surface->size_listVertices, D3D11_BIND_VERTEX_BUFFER, &surface->positionBuffer);
        }
        if (surface->pShader->flagsVertex & D3DVERTEX_NORMAL) {
            engine->GetBM().CreateBuffer(surface->normal.data(), surface->size_normal, surface->size_listNormal, D3D11_BIND_VERTEX_BUFFER, &surface->normalBuffer);
        }
        if (surface->pShader->flagsVertex & D3DVERTEX_COLOR) {
            engine->GetBM().CreateBuffer(surface->color.data(), surface->size_color, surface->size_listColor, D3D11_BIND_VERTEX_BUFFER, &surface->colorBuffer);
        }

        // Indexbuffer bef�llen
        engine->GetBM().CreateBuffer(surface->indices.data(), sizeof(UINT), surface->size_listIndex, D3D11_BIND_INDEX_BUFFER, &surface->indexBuffer);
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

    inline void AddTriangle(LPSURFACE surface, unsigned int a, unsigned int b, unsigned int c)
    {
        surface->AddIndex(a);
        surface->AddIndex(b);
        surface->AddIndex(c);
    }

    inline int Cls(int r, int g, int b, int a = 255) { return static_cast<int>(engine->Cls(float(r / 255.0f), float(g / 255.0f), float(b / 255.0f), float(a / 255.0f))); }

    inline int Flip(int syncInterval = 1) { return static_cast<int>(engine->m_device.Flip(syncInterval)); }

    inline unsigned int Graphics(unsigned int width, unsigned int height, bool windowed = true) { return static_cast<int>(engine->Graphic(width, height, windowed)); }

    inline void RenderWorld() { engine->RenderWorld(); }

    inline void PositionEntity(LPLIGHT light, float x, float y, float z)
    {
        light->PositionEntity(x, y, z);
    }

} // End of namespace Engine
