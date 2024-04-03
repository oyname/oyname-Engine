#include "gidx.h"

using namespace DirectX;

int main()
{
    Engine::Graphics(1024, 768);

    LPMESH camera;
    Engine::CreateCamera(&camera);

    // Brush erstellen / Jedes Brush ist nach dem erstellen ein Child vom Standard-Shader
    LPBRUSH brush;
    Engine::CreateBrush(&brush);

    // Vertexdaten werden hier gespeichert
    LPMESH mesh;
    Engine::CreateMesh(&mesh, brush);

    LPMESH mesh2;
    Engine::CreateMesh(&mesh2, brush);

    // Definiere die Anzahl der zu erstellenden Meshes
    const int numMeshes = 1;

    // Vector zum Speichern der Mesh-Objekte
    std::vector<LPMESH> meshes(numMeshes);

    // Schleife zum Erstellen der Meshes
    for (int i = 0; i < numMeshes; i++) {
        Engine::CreateMesh(&meshes[i], brush);
    }

    // Vertexdaten werden hier gespeichert
    LPSURFACE surface;
    Engine::CreateSurface(&surface, mesh);

    // Schleife zum Hinzufügen des Surface zu allen Meshes
    for (int i = 0; i < numMeshes; i++) {
        Engine::engine->GetMM().addSurfaceToMesh(meshes[i], surface);
    }

    Engine::engine->GetMM().addSurfaceToMesh(mesh2, surface);

    Engine::AddVertex(surface, -1.0f, -1.0f, 1.0f); Engine::VertexColor(surface, 0  , 255, 0);
    Engine::AddVertex(surface, -1.0f,  1.0f, 1.0f); Engine::VertexColor(surface, 0  ,   0, 255);
    Engine::AddVertex(surface,  1.0f, -1.0f, 1.0f); Engine::VertexColor(surface, 255,   0, 0);
    Engine::AddVertex(surface,  1.0f,  1.0f, 1.0f); Engine::VertexColor(surface, 255, 255, 0);

    Engine::AddVertex(surface, -1.0f, -1.0f, -1.0f); Engine::VertexColor(surface, 0, 255, 0);
    Engine::AddVertex(surface, -1.0f, 1.0f,  -1.0f); Engine::VertexColor(surface, 0, 0, 255);
    Engine::AddVertex(surface,  1.0f, -1.0f, -1.0f); Engine::VertexColor(surface, 255, 0, 0);
    Engine::AddVertex(surface,  1.0f, 1.0f,  -1.0f); Engine::VertexColor(surface, 255, 255, 0);

    Engine::AddTriangle(surface, 2, 1, 0);// Seite A
    Engine::AddTriangle(surface, 2, 3, 1);

    Engine::AddTriangle(surface, 4, 5, 6);// Seite B 
    Engine::AddTriangle(surface, 6, 5, 7);

    Engine::AddTriangle(surface, 6, 7, 2);// Seite C 
    Engine::AddTriangle(surface, 2, 7, 3);

    Engine::AddTriangle(surface, 0, 1, 5);// Seite D 
    Engine::AddTriangle(surface, 5, 4, 0);

    Engine::AddTriangle(surface, 5, 1, 7);// Seite E 
    Engine::AddTriangle(surface, 7, 1, 3);

    Engine::AddTriangle(surface, 6, 0, 4);// Seite F 
    Engine::AddTriangle(surface, 6, 2, 0);
    
    Engine::FillBuffer(surface);


    float winkel = 0.0f;

    Engine::TranslationMatrix(mesh, 0.0f, 0.0f, 5.0f);

    while (gdx::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
    {

        Engine::Cls(32, 64, 128);


        winkel = winkel + 0.01f;

        //Engine::RotateMesh(mesh, 0, winkel, winkel, 2.0f);
        //Engine::RotateMesh(mesh2, 0, 45, winkel, -2.0f);

        Engine::RenderWorld();

        Engine::Flip();
    }

    // Shutdown the engine
    return(gdx::ShutDown());
}
