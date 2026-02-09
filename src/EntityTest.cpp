#include "gidx.h"

// Function declaration
static void CreateCube(LPENTITY* mesh, MATERIAL* material = nullptr);

int main2()
{
    bool sw = true;

    sw == true ? Engine::Graphics(1200, 650) : Engine::Graphics(1980, 1080, false);

    LPTEXTURE texture = nullptr;
    Engine::LoadTexture(&texture, L"..\\media\\face.bmp");
    LPMATERIAL material;
    Engine::CreateMaterial(&material);
    Engine::MaterialTexture(material, texture);

    // Light erstellen
    LPENTITY light = nullptr;
    Engine::CreateLight(&light, D3DLIGHT_DIRECTIONAL);  // Nutzt LightManager
    Engine::TurnEntity(light, 45, 0, 0);                // Funktioniert

    // Camera erstellen
    LPENTITY camera = nullptr;
    Engine::CreateCamera(&camera);                      // Nutzt ObjectManager
    Engine::PositionEntity(camera, 0, 0, -5);           // Funktioniert

    // Mesh erstellen und rendern
    LPENTITY cube = nullptr;
    Engine::CreateMesh(&cube);                          // Nutzt ObjectManager

    CreateCube(&cube, material);
    Engine::PositionEntity(cube, 0.0f, 0.0f, 5.0f);
    Engine::RotateEntity(cube, 45.0f, 45.0f, 0.0f);
    Engine::ScaleEntity(cube, 1.0f, 1.0f, 1.0f);


    while (Windows::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) // Main loop
    {
        Engine::Cls(0, 64, 128);


        Engine::UpdateWorld();
        Engine::RenderWorld();
        Engine::Flip();
    }

    // Shutdown the engine
    return Windows::ShutDown();
}

void CreateCube(LPENTITY* mesh, MATERIAL* material)
{
    LPSURFACE wuerfel = NULL;

    Engine::CreateMesh(mesh, material);
    Engine::CreateSurface(&wuerfel, (*mesh));

    // Definition der Eckpunkte f�r jede Seite des W�rfels
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);

    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);

    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);

    Engine::VertexTexCoord(wuerfel, 0.0f, 1.0f);
    Engine::VertexTexCoord(wuerfel, 0.0f, 0.0f);
    Engine::VertexTexCoord(wuerfel, 1.0f, 1.0f);
    Engine::VertexTexCoord(wuerfel, 1.0f, 0.0f);

    Engine::VertexTexCoord(wuerfel, 0.0f, 1.0f);
    Engine::VertexTexCoord(wuerfel, 0.0f, 0.0f);
    Engine::VertexTexCoord(wuerfel, 1.0f, 1.0f);
    Engine::VertexTexCoord(wuerfel, 1.0f, 0.0f);

    Engine::VertexTexCoord(wuerfel, 0.0f, 1.0f);
    Engine::VertexTexCoord(wuerfel, 0.0f, 0.0f);
    Engine::VertexTexCoord(wuerfel, 1.0f, 1.0f);
    Engine::VertexTexCoord(wuerfel, 1.0f, 0.0f);

    Engine::VertexTexCoord(wuerfel, 0.0f, 1.0f);
    Engine::VertexTexCoord(wuerfel, 0.0f, 0.0f);
    Engine::VertexTexCoord(wuerfel, 1.0f, 1.0f);
    Engine::VertexTexCoord(wuerfel, 1.0f, 0.0f);

    Engine::VertexTexCoord(wuerfel, 0.0f, 1.0f);
    Engine::VertexTexCoord(wuerfel, 0.0f, 0.0f);
    Engine::VertexTexCoord(wuerfel, 1.0f, 1.0f);
    Engine::VertexTexCoord(wuerfel, 1.0f, 0.0f);

    Engine::VertexTexCoord(wuerfel, 0.0f, 1.0f);
    Engine::VertexTexCoord(wuerfel, 0.0f, 0.0f);
    Engine::VertexTexCoord(wuerfel, 1.0f, 1.0f);
    Engine::VertexTexCoord(wuerfel, 1.0f, 0.0f);

    // Definition der Dreiecke f�r jede Seite des W�rfels
    Engine::AddTriangle(wuerfel, 0, 1, 2);
    Engine::AddTriangle(wuerfel, 3, 2, 1);

    Engine::AddTriangle(wuerfel, 6, 5, 4);
    Engine::AddTriangle(wuerfel, 6, 7, 5);

    Engine::AddTriangle(wuerfel, 8, 9, 10);
    Engine::AddTriangle(wuerfel, 10, 9, 11);

    Engine::AddTriangle(wuerfel, 14, 13, 12);
    Engine::AddTriangle(wuerfel, 14, 15, 13);

    Engine::AddTriangle(wuerfel, 16, 17, 18);
    Engine::AddTriangle(wuerfel, 18, 17, 19);

    Engine::AddTriangle(wuerfel, 21, 22, 23);
    Engine::AddTriangle(wuerfel, 22, 21, 20);

    Engine::FillBuffer(wuerfel);
}