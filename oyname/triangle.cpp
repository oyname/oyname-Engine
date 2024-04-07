#include "gidx.h"

using namespace DirectX;

int main()
{
    Engine::Graphics(1024, 768);

    LPCAMERA camera;
    Engine::CreateCamera(&camera);

    // Brush erstellen / Jedes Brush ist nach dem erstellen ein Child vom Standard-Shader
    LPBRUSH brush;
    Engine::CreateBrush(&brush);

    // Vertexdaten werden hier gespeichert
    LPMESH mesh;
    Engine::CreateMesh(&mesh, brush);

    LPMESH mesh2;
    Engine::CreateMesh(&mesh2, brush);

    LPMESH mesh3;
    Engine::CreateMesh(&mesh3, brush);

    // Vertexdaten werden hier gespeichert
    LPSURFACE wuerfel;
    Engine::CreateSurface(&wuerfel, mesh);
    Engine::engine->GetOM().addSurfaceToMesh(mesh2, wuerfel);
    Engine::engine->GetOM().addSurfaceToMesh(mesh3, wuerfel);

    // Definition der Eckpunkte für jede Seite des Würfels
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel,  128, 0, 64);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel,   128, 0, 64);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 128, 0, 64);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 128, 0, 64);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 0, 255, 0);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 0, 255, 0);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 0, 255, 0);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 0, 255, 0);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 255, 0, 0);
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 255, 0, 0);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 255, 0, 0);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 255, 0, 0);

    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 255, 128, 64);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 255, 128, 64);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 255, 128, 64);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 255, 128, 64);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 255, 165, 0);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 255, 165, 0);
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 255, 165, 0);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 255, 165, 0);

    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel, 255, 0, 0);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel, 0, 0, 255);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel, 0, 255, 0);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel, 0, 255, 255);

    // Definition der Dreiecke für jede Seite des Würfels
    Engine::AddTriangle(wuerfel, 0, 1, 2);  // Seite A
    Engine::AddTriangle(wuerfel, 2, 1, 3);

    Engine::AddTriangle(wuerfel, 6, 7, 4);  // Seite B
    Engine::AddTriangle(wuerfel, 4, 7, 5);

    Engine::AddTriangle(wuerfel, 8, 9, 10);  // Seite C
    Engine::AddTriangle(wuerfel, 10, 9, 11);

    Engine::AddTriangle(wuerfel, 12, 14, 13); // Seite D
    Engine::AddTriangle(wuerfel, 13, 14, 15);

    Engine::AddTriangle(wuerfel, 16, 17, 18);  // Seite E
    Engine::AddTriangle(wuerfel, 18, 17, 19);

    Engine::AddTriangle(wuerfel, 20, 22, 21);  // Seite F
    Engine::AddTriangle(wuerfel, 22, 23, 21);

    Engine::FillBuffer(wuerfel);

    camera->PositionEntity(0.0f, 2.0f, -8.0f);
    camera->RotateEntity(10.0f, 0.0f, 0.0f);

    //mesh->RotateEntity(0.0f, 30.0f, 0.0f);
    mesh->PositionEntity(-5.0f, 0.0f, 0.0f);

    //mesh2->RotateEntity(0.0f, 0.0f, 0.0f);
    mesh2->PositionEntity(5.0f, 0.0f, 5.0f);

    LPLIGHT light;
    Engine::CreateLight(&light);

    Engine::PositionEntity(light, 10.0f, 10.0f, 0.0f);

    //Engine::engine->SetCamera(mesh3);
    
    while (gdx::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
    {
        Engine::Cls(32, 64, 128);

        mesh->TurnEntity(0.0f, 0.0f, 1.0f);
        mesh2->TurnEntity(1.0f, 0.0f, 0.0f);
        mesh3->TurnEntity(0.0f, 1.0f, 0.0f);

        Engine::RenderWorld();

        Engine::Flip();
    }

    // Shutdown the engine
    return(gdx::ShutDown());
}
