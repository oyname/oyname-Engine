#include "gidx.h"

// Function declaration
void CreateCube(LPMESH* mesh, BRUSH* brush = nullptr);

int main()
{
    Engine::Graphics(1024, 768);

    LPTEXTURE texture1 = nullptr;
    Engine::LoadTexture(&texture1, L"..\\oyname\\Texture\\test4.bmp");

    LPTEXTURE texture2 = nullptr;
    Engine::LoadTexture(&texture2, L"..\\oyname\\Texture\\test2.bmp");

    LPTEXTURE texture3 = nullptr;
    Engine::LoadTexture(&texture3, L"..\\oyname\\Texture\\test.bmp");

    // Das erste Brush wird von der Engine bei Start erstellt

    LPBRUSH brush2;
    Engine::CreateBrush(&brush2);
    
    LPBRUSH brush3;
    Engine::CreateBrush(&brush3);

    // Creating camera object
    LPCAMERA camera;
    Engine::CreateCamera(&camera);
    // Positioning the camera
    Engine::RotateEntity(camera, 60.0f, 0.0f, 0.0f);
    Engine::MoveEntity(camera, 0.0f, 0.0f, -7.0f);

    // Creating light
    LPLIGHT light;
    Engine::CreateLight(&light, D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL);
    Engine::RotateEntity(light, 0.0f, 0.0f, 0.0f);

    // Create cube
    LPMESH cube;
    CreateCube(&cube);
    Engine::EntityTexture(cube, texture1); // Die Textur wird zum Brush, dass zu cube gehört angehängt. In diesem Fall dem Standard-Brush
    
    Engine::PositionEntity(cube, 0.0f, 0.0f, 0.0f);
    Engine::RotateEntity(cube, 20.0f, 20.0f, 40.0f);

    LPMESH cube2;
    CreateCube(&cube2, brush2);

    Engine::BrushTexture(brush2, texture2); // So kann man auch einem Brush eine Textur zuweisen

    Engine::PositionEntity(cube2, -4.0f, 0.0f, 0.0f);
    Engine::RotateEntity(cube2, 20.0f, 20.0f, 40.0f);

    LPMESH cube3;
    CreateCube(&cube3, brush3);

    Engine::BrushTexture(brush3, texture3);

    Engine::PositionEntity(cube3, 4.0f, 0.0f, 0.0f);
    Engine::RotateEntity(cube3, 20.0f, 20.0f, 40.0f);

    while (gdx::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) // Main loop
    {
        Engine::Cls(80, 80, 80);

        Engine::TurnEntity(cube, 0.3f, 0.0, 0.1f);
        Engine::TurnEntity(cube2, -0.3f, 0.0, -0.1f);
        Engine::TurnEntity(cube3, 0.3f, -0.1, 0.0f);

        Engine::RenderWorld();

        Engine::Flip();
    }

    // Shutdown the engine
    return gdx::ShutDown();
}

// Function definitions
void CreateCube(LPMESH* mesh, BRUSH* brush)
{
    LPSURFACE wuerfel = NULL;

    Engine::CreateMesh(mesh, brush);
    Engine::CreateSurface(&wuerfel, (*mesh));

    // Definition der Eckpunkte für jede Seite des Würfels
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

    // Definition der Dreiecke für jede Seite des Würfels
    Engine::AddTriangle(wuerfel, 0, 1, 2);  // Seite A
    Engine::AddTriangle(wuerfel, 3, 2, 1);

    Engine::AddTriangle(wuerfel, 6, 5, 4);  // Seite B
    Engine::AddTriangle(wuerfel, 6, 7, 5);

    Engine::AddTriangle(wuerfel, 8, 9, 10);  // Seite C
    Engine::AddTriangle(wuerfel, 10, 9, 11);

    Engine::AddTriangle(wuerfel, 14, 13, 12); // Seite D
    Engine::AddTriangle(wuerfel, 14, 15, 13);

    Engine::AddTriangle(wuerfel, 16, 17, 18);  // Seite E
    Engine::AddTriangle(wuerfel, 18, 17, 19);

    Engine::AddTriangle(wuerfel, 21, 22, 23);  // Seite F
    Engine::AddTriangle(wuerfel, 22, 21, 20);

    Engine::FillBuffer(wuerfel);
}