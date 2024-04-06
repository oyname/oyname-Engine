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

        *lplpShader = engine->GetMM().createShader();
        
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
        *lplpBrush = engine->GetMM().createBrush();
        shader = shader == nullptr ? engine->GetSM().GetStandardShader() : shader;
        engine->GetMM().addBrushToShader(shader, *lplpBrush);
    }

    inline void CreateMesh(LPMESH *mesh, BRUSH* lpBrush) 
    {
        *mesh = engine->GetMM().createMesh();
        engine->GetMM().addMeshToBrush(lpBrush, *mesh);

        XMMATRIX mRotation = XMMatrixIdentity();
        XMMATRIX mScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
        XMMATRIX mTranslation = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

        (*mesh)->cb.viewMatrix = engine->GetCam().GetCurrentCam()->viewMatrix();
        (*mesh)->cb.projectionMatrix = engine->GetCam().GetCurrentCam()->projectionMatrix();

        engine->GetBM().CreateBuffer(&(*mesh)->cb, sizeof(ConstantBuffer), 1, D3D11_BIND_CONSTANT_BUFFER, &(*mesh)->constantBuffer);
    }

    inline void PositionEntity(LPMESH mesh, float x, float y, float z)
    {
        // Aktualisiere die Position des Meshes um den Translationsvektor
        mesh->position = XMVectorSet(x, y, z, 0.0f);

        XMMATRIX mTranslation = XMMatrixTranslationFromVector(mesh->position);
        
        mesh->mTranslation = mTranslation;
        //mesh->mTranslation = XMMatrixTranslationFromVector(mesh->position);
    }

    inline void MoveEntity(LPMESH mesh, float x, float y, float z)
    {
        // Erstellen eines Verschiebungsvektors basierend auf den gegebenen Komponenten
        XMVECTOR translation = XMVectorSet(x, y, z, 0.0f);

        // Rotation der Verschiebung entsprechend der lokalen Ausrichtung des Objekts
        translation = XMVector3TransformCoord(translation, mesh->mRotate);

        // Addieren des transformierten Verschiebungsvektors zur aktuellen Position des Meshes
        mesh->position = XMVectorAdd(mesh->position, translation);

        // Erstellen der Translationsmatrix für die transformierte Verschiebung
        XMMATRIX mTranslation = XMMatrixTranslationFromVector(translation);

        // Multiplizieren der aktuellen Translationsmatrix des Meshes mit der neuen Translationsmatrix
        mesh->mTranslation *= mTranslation;
    }

    inline void RotateEntity(LPMESH lpMesh, float fRotateX, float fRotateY, float fRotateZ)
    {
        // Konvertieren der Rotationswinkel von Grad in Radiant
        fRotateX = XMConvertToRadians(fRotateX);
        fRotateY = XMConvertToRadians(fRotateY);
        fRotateZ = XMConvertToRadians(fRotateZ);

        // Erstellen der Rotationsmatrizen für jede Achse
        XMMATRIX rotationX = XMMatrixRotationX(fRotateX);
        XMMATRIX rotationY = XMMatrixRotationY(fRotateY);
        XMMATRIX rotationZ = XMMatrixRotationZ(fRotateZ);
        // Kombinieren der Rotationsmatrizen
        XMMATRIX rotationMatrix = rotationX * rotationY * rotationZ;

        // Wenden Sie die Rotation auf die lokale Rotationsmatrix des Objekts an
        XMMATRIX newRotationMatrix = rotationMatrix;// *lpMesh->cb.worldMatrix;

        lpMesh->mRotate = rotationMatrix;
    }

    // Funktion zur Rotation eines Objekts um seine lokalen Achsen
    inline void TurnEntity(LPMESH lpMesh, float fRotateX, float fRotateY, float fRotateZ)
    {
        // Konstruieren Sie die Rotationsmatrizen um die lokalen Achsen
        XMMATRIX rotationX = XMMatrixRotationX(XMConvertToRadians(fRotateX));
        XMMATRIX rotationY = XMMatrixRotationY(XMConvertToRadians(fRotateY));
        XMMATRIX rotationZ = XMMatrixRotationZ(XMConvertToRadians(fRotateZ));

        // Kombinieren Sie die Rotationsmatrizen in eine Gesamtrotationsmatrix
        XMMATRIX rotationMatrix = rotationX * rotationY * rotationZ;

        // Wenden Sie die Rotation auf die lokale Rotationsmatrix des Objekts an
        XMMATRIX newRotationMatrix = rotationMatrix  * lpMesh->mRotate;

        lpMesh->mRotate = newRotationMatrix;
    }

    inline void CreateCamera(LPMESH* camera)
    {
        engine->GetCam().SetPerspective(XMConvertToRadians(75.0f),
                                        (static_cast<float>(engine->GetWidth()) / static_cast<float>(engine->GetHeight())),
                                        0.01f,
                                        1000.0f);

        // Kamera erstellen.
        engine->GetCam().CreateCamera(camera); 

        // Engine speichert aktuelle Kamera  
        engine->SetCamera(*camera); 
    }

    inline void CreateSurface(SURFACE** lplpSurface, MESH* lpMesh){
        *lplpSurface = engine->GetMM().createSurface();
        engine->GetMM().addSurfaceToMesh(lpMesh, *lplpSurface);
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

    inline void RenderWorld() { engine->RenderWorld(); }

} // End of namespace Engine
