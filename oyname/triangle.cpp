#include "gidx.h"

int main()
{
    int a = 10;
    a = a + 10;

    Engine::Graphics(1024, 768);

    LPMESH camera;
    Engine::CreateCamera(&camera);

    // Brush erstellen / Jedes Brush ist nach dem erstellen ein Child vom Standard-Shader
    LPBRUSH brush;
    Engine::CreateBrush(&brush);

    // Vertexdaten werden hier gespeichert
    LPMESH mesh;
    Engine::CreateMesh(&mesh, brush);

    // Vertexdaten werden hier gespeichert
    LPSURFACE surface;
    Engine::CreateSurface(&surface, mesh);

    Engine::AddVertex(surface, -1.0f, -1.0f, 0.0f); Engine::VertexColor(surface, 0  , 255, 0);
    Engine::AddVertex(surface, -1.0f,  1.0f, 0.0f); Engine::VertexColor(surface, 0  ,   0, 255);
    Engine::AddVertex(surface,  1.0f, -1.0f, 0.0f); Engine::VertexColor(surface, 255,   0, 0);
    Engine::AddVertex(surface,  1.0f,  1.0f, 0.0f); Engine::VertexColor(surface, 255, 255, 0);

    Engine::AddTriangle(surface, 0, 1, 2);
    Engine::AddTriangle(surface, 1, 3, 2);
    
    Engine::FillBuffer(surface);

    while (gdx::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
    {
        Engine::Cls(32, 64, 128);

        Engine::RenderWorld();

        Engine::Flip();
    }

    // Shutdown the engine
    return(gdx::ShutDown());
}
