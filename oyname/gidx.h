#pragma once

#include <windows.h>
#include <DirectXMath.h>

#include "gdxengine.h"


namespace Engine
{
    extern gdx::CGIDX* engine;

    // 
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

    //
    inline HRESULT CreateShader(SHADER** lplpShader, const std::wstring& vertexShaderFile, const std::wstring& pixelShaderFile)
    {   
         HRESULT result = S_OK;

        *lplpShader = engine->GetOM().createShader();
        
        // Shader erstellen und laden
        engine->GetSM().CreateShader(*lplpShader, vertexShaderFile, pixelShaderFile);
        if (FAILED(result)) {
            return result;
        }
        // LAYOUT ERSTELLEN
        result = Engine::engine->GetILM().CreateInputLayout(*lplpShader, D3DVERTEX_POSITION);
        if (FAILED(result)) {
            return result;
        }
    }

    inline void CreateBrush(BRUSH** lplpBrush, SHADER* shader = nullptr) {
        *lplpBrush = engine->GetOM().createBrush();
        shader = shader == nullptr ? engine->GetSM().GetStandardShader() : shader;
        engine->GetOM().addBrushToShader(shader, *lplpBrush);
    }

    inline void CreateMesh(LPMESH *mesh, BRUSH* lpBrush) {
        *mesh = engine->GetOM().createMesh();
        engine->GetOM().addMeshToBrush(lpBrush, *mesh);

        XMFLOAT3 scale(1.0f, 1.0f, 1.0f);
        XMFLOAT3 position(0.0f, 0.0f, 3.0f);

        XMVECTOR rotationAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        XMMATRIX mRotation = XMMatrixRotationAxis(rotationAxis, 0.0f);

        XMMATRIX mScale = XMMatrixScaling(scale.x, scale.y, scale.z);
        XMMATRIX mTranslation = XMMatrixTranslation(position.x, position.y, position.z);

        XMMATRIX mWorld = mScale * mRotation * mTranslation;

        (*mesh)->cb.viewMatrix = engine->GetCam().GetView()->cb.viewMatrix;
        (*mesh)->cb.projectionMatrix = engine->GetCam().GetView()->cb.projectionMatrix;
        (*mesh)->cb.worldMatrix = mScale * mRotation * mTranslation;

        engine->GetBM().CreateBuffer(&(*mesh)->cb, sizeof(ConstantBuffer), 1, D3D11_BIND_CONSTANT_BUFFER, &(*mesh)->constantBuffer);
    }

    inline void CreateCamera(LPMESH* camera, bool perspektive = true)
    {
        // Kamera erstellen. Das macht der Objectmanager
        *camera = engine->GetOM().createMesh(); 
 
        engine->GetCam().SetPerspective(XMConvertToRadians(75.0f),
                                        (static_cast<float>(engine->GetWidth()) / static_cast<float>(engine->GetHeight())),
                                        0.01f,
                                        1000.0f);

        engine->GetCam().CreateCamera(camera);

        // Engine speichert aktuelle Kamera  
        engine->SetCamera(*camera); 
    }

    inline void CreateSurface(SURFACE** lplpSurface, MESH* lpMesh){
        *lplpSurface = engine->GetOM().createSurface();
        engine->GetOM().addSurfaceToMesh(lpMesh, *lplpSurface);
    }

    inline void FillBuffer(SURFACE* surface) {
        // Mit Vertexdaten befüllen
        if (surface->pShader->flags & D3DVERTEX_POSITION) {
            engine->GetBM().CreateBuffer(surface->position.data(), surface->size_vertex, surface->size_listVertices, D3D11_BIND_VERTEX_BUFFER, &surface->positionBuffer);
        }
        if (surface->pShader->flags & D3DVERTEX_COLOR) {
            engine->GetBM().CreateBuffer(surface->color.data(), surface->size_color, surface->size_listColor, D3D11_BIND_VERTEX_BUFFER, &surface->colorBuffer);
        }

        // Indexbuffer befüllen
        engine->GetBM().CreateBuffer(surface->indices.data(), sizeof(UINT), surface->size_listIndex, D3D11_BIND_INDEX_BUFFER, &surface->indexBuffer);
    }
    
    inline void AddVertex(SURFACE* lpSurface, float x, float y, float z)
    {
        Engine::engine->GetBM().AddVertex(lpSurface, x, y, z);
    }

    inline void VertexColor(SURFACE* lpSurface, unsigned int r, unsigned int g, unsigned int b)
    {
        Engine::engine->GetBM().AddColor(lpSurface, float(r / 255.0f), float(g / 255.0f), float(b / 255.0f));
    }

    inline void AddTriangle(SURFACE* lpSurface, unsigned int a, unsigned int b, unsigned int c)
    {
        Engine::engine->GetBM().AddIndex(lpSurface, a);
        Engine::engine->GetBM().AddIndex(lpSurface, b);
        Engine::engine->GetBM().AddIndex(lpSurface, c);
    }

    inline int Cls(int r, int g, int b, int a = 255) { return static_cast<int>(engine->Cls(float(r / 255.0f), float(g / 255.0f), float(b / 255.0f), float(a / 255.0f))); }

    inline int Flip(int syncInterval = 1) { return static_cast<int>(engine->m_device.Flip(syncInterval)); }

    inline unsigned int Graphics(unsigned int width, unsigned int height, bool windowed = true) { return static_cast<int>(engine->Graphic(width, height, windowed)); }

} // End of namespace Engine
