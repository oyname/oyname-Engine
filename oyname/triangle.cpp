#include "gidx.h"

using namespace DirectX;

LPMESH g_mesh3;
void CreateCubex(LPMESH* mesh, BRUSH* brush);

// Funktion, um das Objekt um einen Punkt im Kreis zu bewegen
void MoveObjectInCircle(float centerX, float centerZ, float radius, float angle);

int main_off3()
{
    Engine::Graphics(1024, 768);

    LPCAMERA camera;
    Engine::CreateCamera(&camera);

    LPSHADER shader2;
    Engine::CreateShader(&shader2, L"vertexshader.hlsl", L"pixelshader.hlsl", "main2", D3DVERTEX_POSITION | D3DVERTEX_COLOR | D3DVERTEX_NORMAL);

    // Brush erstellen / Jedes Brush ist nach dem erstellen ein Child vom Standard-Shader
    LPBRUSH brush;
    Engine::CreateBrush(&brush);
    
    LPBRUSH brush2;
    Engine::CreateBrush(&brush2, shader2);

    // Vertexdaten werden hier gespeichert
    LPMESH mesh;
    Engine::CreateMesh(&mesh, brush2);

    LPMESH mesh2;
    //Engine::CreateMesh(&mesh2, brush);
    CreateCubex(&mesh2,nullptr);

    LPSURFACE test = Engine::GetSurface(mesh2);
    Debug::Log("TEST: ", test);

    //global
    Engine::CreateMesh(&g_mesh3, brush);

    LPMESH lightMesh;
    Engine::CreateMesh(&lightMesh, brush);

    // Vertexdaten werden hier gespeichert
    LPSURFACE wuerfel;
    Engine::CreateSurface(&wuerfel, mesh);
    Engine::engine->GetOM().addSurfaceToMesh(g_mesh3, test);
    Engine::engine->GetOM().addSurfaceToMesh(g_mesh3, wuerfel);
    Engine::engine->GetOM().addSurfaceToMesh(lightMesh, wuerfel);

    LPLIGHT light;
    Engine::CreateLight(&light, D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL);

    // Definition der Eckpunkte f�r jede Seite des W�rfels
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 0, 255, 128, 64);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 1, 255, 128, 64);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 2, 255, 128, 64);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 3, 255, 128, 64);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 4, 0, 255, 0);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 5, 0, 255, 0);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 6, 0, 255, 0);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 7, 0, 255, 0);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 8, 32, 255, 128);
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 9, 32, 255, 128);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 10, 32, 255, 128);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 11, 32, 255, 128);

    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 12, 255, 0, 64);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 13, 255, 0, 64);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 14, 255, 0, 64);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 15, 255, 0, 64);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 16, 255, 200, 0);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 17, 255, 200, 0);
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 18, 255, 200, 0);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 19, 255, 200, 0);

    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel, 20, 255, 0, 0);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel, 21, 0, 0, 255);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel, 22, 0, 255, 0);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel, 23, 0, 255, 255);

    // Definition der Dreiecke f�r jede Seite des W�rfels
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

    camera->PositionEntity(15.0f, 0.0f, -5.0f);
    camera->RotateEntity(-10.0f, -60.0f, 0.0f);

    //mesh->RotateEntity(0.0f, 30.0f, 0.0f);
    mesh->PositionEntity(-5.0f, 5.0f, 0.0f);

    //mesh2->RotateEntity(0.0f, 0.0f, 0.0f);
    mesh2->PositionEntity(5.0f, 0.0f, 5.0f);

    Engine::PositionEntity(light, -15.0f, 10.0f, 0.0f);
    Engine::PositionEntity(lightMesh, -15.0f, 10.0f, 0.0f);

    lightMesh->ScaleEntity(0.5f, 0.1f, 0.5f);
    g_mesh3->ScaleEntity(1.0f, 1.0f, 1.0f);
    g_mesh3->PositionEntity(0.0f, 0.0f, -5.0f);
    Engine::RotateEntity(g_mesh3, 0.0f, 315.0f, 0.0f);

    //Engine::engine->SetCamera(mesh3);
    
    // Definieren des Mittelpunkts und des Radius des Kreises
    float centerX = 0.0f;
    float centerZ = 0.0f;
    float radius = 12.0f; // Beispielradius von 5 Einheiten

    // Winkel inkrementieren, um das Objekt im Kreis zu bewegen
    float angle = 0.0f; // Startwinkel

    while (gdx::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
    {
        Engine::Cls(16, 16, 64);

        MoveObjectInCircle(centerX, centerZ, radius, angle);

        // Inkrementiere den Winkel f�r die n�chste Position im Kreis
        angle += 0.01f; // Beispielinkrement

        light->MoveEntity(0.01f, -0.00f, 0.0f);
        lightMesh->MoveEntity(0.01f, -0.00f, 0.0f);

        mesh->TurnEntity(1.0f, 0.0f, 1.0f);
        mesh2->TurnEntity(0.0f, 0.0f, 0.5f);

        Engine::TurnEntity(g_mesh3, 0.5f, 0.5f, 0.0f);

        Engine::RenderWorld();

        Engine::Flip();
    }

    // Shutdown the engine
    return(gdx::ShutDown());
}


// Funktion, um das Objekt um einen Punkt im Kreis zu bewegen
void MoveObjectInCircle(float centerX, float centerZ, float radius, float angle) {
    // Berechnen der neuen Position basierend auf dem Winkel
    float newX = centerX + radius * cos(angle);
    float newZ = centerZ + radius * sin(angle);

    g_mesh3->TurnEntity(0.0f, -0.68f, 0.0f);

    // Setzen der neuen Position des Objekts
    g_mesh3->PositionEntity(newX, 0.0f, newZ);
}

void CreateCubex(LPMESH* mesh, BRUSH* brush = nullptr)
{
    LPSURFACE wuerfel = NULL;

    Engine::CreateMesh(mesh,brush);
    Engine::CreateSurface(&wuerfel, (*mesh));

    // Definition der Eckpunkte f�r jede Seite des W�rfels
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 0, 255, 128, 64);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 1, 255, 128, 64);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 2, 255, 128, 64);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 3, 255, 128, 64);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 4, 0, 255, 0);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 5, 0, 255, 0);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 6, 0, 255, 0);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 7, 0, 255, 0);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 8, 32, 255, 128);
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 9, 32, 255, 128);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 10, 32, 255, 128);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 11, 32, 255, 128);

    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 12, 255, 0, 64);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 13, 255, 0, 64);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 14, 255, 0, 64);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 15, 255, 0, 64);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 16, 255, 200, 0);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 17, 255, 200, 0);
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 18, 255, 200, 0);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 19, 255, 200, 0);

    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel, 20, 255, 0, 0);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel, 21, 0, 0, 255);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel, 22, 0, 255, 0);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel, 23, 0, 255, 255);

    // Definition der Dreiecke f�r jede Seite des W�rfels
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
}