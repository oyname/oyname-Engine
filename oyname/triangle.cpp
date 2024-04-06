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

    LPMESH mesh3;
    Engine::CreateMesh(&mesh3, brush);

    // Vertexdaten werden hier gespeichert
    LPSURFACE wuerfel;
    Engine::CreateSurface(&wuerfel, mesh);

    LPSURFACE pyramide;
    Engine::CreateSurface(&pyramide, mesh3);

    Engine::engine->GetOM().addSurfaceToMesh(mesh2, wuerfel);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f); Engine::VertexColor(wuerfel, 0  , 255, 0);
    Engine::AddVertex(wuerfel, -1.0f,  1.0f, 1.0f); Engine::VertexColor(wuerfel, 0  ,   0, 255);
    Engine::AddVertex(wuerfel,  1.0f, -1.0f, 1.0f); Engine::VertexColor(wuerfel, 255,   0, 0);
    Engine::AddVertex(wuerfel,  1.0f,  1.0f, 1.0f); Engine::VertexColor(wuerfel, 255, 255, 0);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexColor(wuerfel, 0, 255, 0);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f,  -1.0f); Engine::VertexColor(wuerfel, 0, 0, 255);
    Engine::AddVertex(wuerfel,  1.0f, -1.0f, -1.0f); Engine::VertexColor(wuerfel, 255, 0, 0);
    Engine::AddVertex(wuerfel,  1.0f, 1.0f,  -1.0f); Engine::VertexColor(wuerfel, 255, 255, 0);

    Engine::AddTriangle(wuerfel, 2, 1, 0);// Seite A
    Engine::AddTriangle(wuerfel, 2, 3, 1);

    Engine::AddTriangle(wuerfel, 4, 5, 6);// Seite B 
    Engine::AddTriangle(wuerfel, 6, 5, 7);

    Engine::AddTriangle(wuerfel, 6, 7, 2);// Seite C 
    Engine::AddTriangle(wuerfel, 2, 7, 3);

    Engine::AddTriangle(wuerfel, 0, 1, 5);// Seite D 
    Engine::AddTriangle(wuerfel, 5, 4, 0);

    Engine::AddTriangle(wuerfel, 5, 1, 7);// Seite E 
    Engine::AddTriangle(wuerfel, 7, 1, 3);

    Engine::AddTriangle(wuerfel, 6, 0, 4);// Seite F 
    Engine::AddTriangle(wuerfel, 6, 2, 0);
    
    Engine::FillBuffer(wuerfel);

    Engine::AddVertex(pyramide, -1.0f, -1.0f, -1.0f); Engine::VertexColor(pyramide, 0, 255, 0);
    Engine::AddVertex(pyramide, -1.0f, 1.0f, -1.0f); Engine::VertexColor(pyramide, 0, 0, 255);
    Engine::AddVertex(pyramide, 1.0f, -1.0f, -1.0f); Engine::VertexColor(pyramide, 255, 0, 0);
    Engine::AddVertex(pyramide, 1.0f, 1.0f, -1.0f); Engine::VertexColor(pyramide, 255, 255, 0);
    Engine::AddVertex(pyramide, 0.0f, 0.0f, 1.0f); Engine::VertexColor(pyramide, 255, 255, 0);

    Engine::AddTriangle(pyramide, 0, 1, 3);
    Engine::AddTriangle(pyramide, 3, 2, 0);
    Engine::AddTriangle(pyramide, 2, 3, 4);
    Engine::AddTriangle(pyramide, 3, 1, 4);
    Engine::AddTriangle(pyramide, 0, 4, 1);
    Engine::AddTriangle(pyramide, 0, 2, 4);

    Engine::FillBuffer(pyramide);

    camera->PositionEntity(0.0f, 10.0f, -15.0f);
    camera->RotateEntity(35.0f, 0.0f, 0.0f);

    mesh->RotateEntity(0.0f, 30.0f, 0.0f);
    mesh->PositionEntity(-5.0f, 0.0f, 0.0f);

    mesh2->RotateEntity(0.0f, 0.0f, 0.0f);
    mesh2->PositionEntity(0.0f, 0.0f, 5.0f);

    mesh3->RotateEntity(-90.0f, 0.0f, 0.0f);
    mesh3->PositionEntity(5.0f, 0.0f, 0.0f);

    mesh3->RotateEntity(0, -90, 0);

    //Engine::engine->SetCamera(mesh3);
    
    while (gdx::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
    {
        Engine::Cls(32, 64, 128);

        mesh->TurnEntity(0.0f, 0.0f, 1.0f);

        mesh2->TurnEntity(1.0f, 0.0f, 0.0f);
        mesh3->TurnEntity(0.0f, 0.0f, 1.0f);
        mesh3->MoveEntity(0.0f, 0.0f, 0.1f);
        mesh3->TurnEntity(0.0f, 0.0f, 0.5f, Space::World);

        Engine::RenderWorld();

        Engine::Flip();
    }

    // Shutdown the engine
    return(gdx::ShutDown());
}
