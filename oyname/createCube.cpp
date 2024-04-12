#include "gidx.h"

// OYNAME Engine
// 
// Here's a simple example demonstrating how to create vertices and connect 
// them to form a cube. Additionally, it shows how to load another shader to 
// the standard shader, which is automatically generated each time the engine 
// starts, and link it to one of the created cubes.

// Function declaration
void CreateCube(LPMESH* mesh, BRUSH* brush = nullptr);

int main()
{
    Engine::Graphics(1024, 768);

    // Creating camera object
    LPCAMERA camera;
    Engine::CreateCamera(&camera);
    // Positioning the camera
    Engine::PositionEntity(camera, 0.0f, 0.0f, -10.0f);

    // Creating light
    LPLIGHT light;
    Engine::CreateLight(&light);
    // Positioning the light
    Engine::PositionEntity(light, 0.0f, -5.0f, 0.0f);

    // The engine automatically creates a default shader and a 
    // default brush upon startup.In this example, I'm not creating 
    // either of these objects. Consequently, every mesh created 
    // is automatically attached to the default brush, which 
    // is linked to the default shader.

    // Create cube
    LPMESH cube;
    CreateCube(&cube);
    // Positioning the cube
    Engine::PositionEntity(cube, 0.0f, 0.0f, 0.0f);


    while (gdx::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) // Main loop
    {
        Engine::Cls(0, 64, 128);

        // Rotate cube
        Engine::TurnEntity(cube, 0.5f, 0.0f, 0.5f);

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
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 255, 128, 64);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 255, 128, 64);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 255, 128, 64);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 255, 128, 64);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 0, 255, 0);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 0, 255, 0);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 0, 255, 0);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 0, 255, 0);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 32, 255, 128);
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 32, 255, 128);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 32, 255, 128);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel, 32, 255, 128);

    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 255, 0, 64);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 255, 0, 64);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 255, 0, 64);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 255, 0, 64);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 255, 200, 0);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 255, 200, 0);
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 255, 200, 0);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel, 255, 200, 0);

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
}