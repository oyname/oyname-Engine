#include "gidx.h"

// OYNAME Engine
// 
// Here's a simple example demonstrating how to create vertices and connect 
// them to form a cube. Additionally, it shows how to load another shader to 
// the standard shader, which is automatically generated each time the engine 
// starts, and link it to one of the created cubes.

// Function declaration
void CreateCube(LPMESH* mesh, BRUSH* brush = nullptr);
void CreateSphere(LPMESH* mesh, BRUSH* brush, int numSegments);
void CreateCylinder(LPMESH* mesh, BRUSH* brush, int numSegments);
void MoveObjectInCircle(LPMESH mesh, float centerX, float centerZ, float radius, float angle);

int main()
{
    Engine::Graphics(1024, 768);

    // Creating camera object
    LPCAMERA camera;
    Engine::CreateCamera(&camera);
    // Positioning the camera
    Engine::RotateEntity(camera, 60.0f, 0.0f, 0.0f);
    Engine::MoveEntity(camera, 0.0f, 0.0f, -5.0f);
    Engine::MoveEntity(camera, 0.0f, 0.0f, -5.0f);

    float posLightx = 3.0f;
    float posLighty = 1.0f;
    float posLightz = 1.0f;

    // Creating light
    LPLIGHT light;
    Engine::CreateLight(&light, D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL);
    Engine::PositionEntity(light, posLightx, posLighty, posLightz);
    Engine::RotateEntity(light, 90.0f, 0.0f, 0.0f);

    LPMESH sphere;
    CreateSphere(&sphere, nullptr, 64);
    Engine::PositionEntity(sphere, posLightx, posLighty, posLightz);

    // The engine automatically creates a default shader and a 
    // default brush upon startup.In this example, I'm not creating 
    // either of these objects. Consequently, every mesh created 
    // is automatically attached to the default brush, which 
    // is linked to the default shader.

    // Create cube
    LPMESH cube;
    CreateCube(&cube);
    // Positioning the cube
    Engine::PositionEntity(cube, 0.0f, 0.0f, -3.0f);
    Engine::RotateEntity(cube, 20.0f, 50.0f, 50.0f);

    LPMESH cube2;
    Engine::CreateMesh(&cube2);

    Engine::engine->GetOM().addSurfaceToMesh(cube2, Engine::GetSurface(cube));
    Engine::PositionEntity(cube2, 8.0f, 0.0f, 0.0f);
    Engine::RotateEntity(cube2, 0.0f, 45.0f, 0.0f);

    LPMESH cube3;
    CreateCube(&cube3);
    Engine::PositionEntity(cube3, -8.0f, 0.0f, 0.0f);
    Engine::RotateEntity(cube3, 20.0f, 40.0f, 0.0f);

    LPMESH cylinder;
    CreateCylinder(&cylinder, nullptr, 16);
    Engine::PositionEntity(cylinder, 0.0f, 0.0f, 3.5f);

    // Winkel inkrementieren, um das Objekt im Kreis zu bewegen
    float angle = 0.0f;

    while (gdx::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) // Main loop
    {
        Engine::Cls(0, 0, 0);

        Engine::TurnEntity(light, 0.0f, 0.1f, 0.0f);

        //Engine::TurnEntity(cube, 1.0f, 1.0f, 1.0f, Space::Local);

        //Engine::TurnEntity(cylinder, 0.0f, 0.0f, 1.0f);

        angle += 0.005f;

        Engine::RenderWorld();

        Engine::Flip();
    }

    // Shutdown the engine
    return gdx::ShutDown();
}

// Funktion, um das Objekt um einen Punkt im Kreis zu bewegen
void MoveObjectInCircle(LPMESH mesh, float centerX, float centerZ, float radius, float angle) {
    // Berechnen der neuen Position basierend auf dem Winkel
    float newX = centerX + radius * cos(angle);
    float newZ = centerZ + radius * sin(angle);

    // Setzen der neuen Position des Objekts
    mesh->PositionEntity(newX, 5.0f, newZ);
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

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f); Engine::VertexColor(wuerfel, 224, 224, 224);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel,224, 224, 224);
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel,224, 224, 224);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel,224, 224, 224);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f); Engine::VertexColor(wuerfel,224, 224, 224);

    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel,224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel,224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel,224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel,224, 224, 224);

    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel,224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel,224, 224, 224);
    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel,224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f); Engine::VertexColor(wuerfel,224, 224, 224);

    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel,224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel,224, 224, 224);
    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel,224, 224, 224);
    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);  Engine::VertexColor(wuerfel,224, 224, 224);

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

void CreateSphere(LPMESH* mesh, BRUSH* brush, int numSegments)
{
    LPSURFACE sphere = NULL;
    const float radius = 1.0f;

    Engine::CreateMesh(mesh, brush);
    Engine::CreateSurface(&sphere, (*mesh));

    // Erzeugung der Kugelgeometrie
    for (int i = 0; i <= numSegments; ++i)
    {
        float theta1 = i * 3.14f / numSegments;
        float theta2 = (i + 1) * 3.14f / numSegments;

        for (int j = 0; j <= numSegments; ++j)
        {
            float phi = j * 2 * 3.14f / numSegments;

            float x = radius * sin(theta1) * cos(phi);
            float y = radius * sin(theta1) * sin(phi);
            float z = radius * cos(theta1);

            Engine::AddVertex(sphere, x, y, z);
            Engine::VertexNormal(sphere, x / radius, y / radius, z / radius); // Normals for smoothing
            Engine::VertexColor(sphere, - 1, 255, 255, 255);
        }
    }

    // Definition der Dreiecke für die Kugel
    for (int i = 0; i < numSegments; ++i)
    {
        for (int j = 0; j < numSegments; ++j)
        {
            int p0 = i * (numSegments + 1) + j;
            int p1 = p0 + 1;
            int p2 = (i + 1) * (numSegments + 1) + j;
            int p3 = p2 + 1;

            Engine::AddTriangle(sphere, p0, p2, p1);
            Engine::AddTriangle(sphere, p2, p3, p1);
        }
    }

    Engine::FillBuffer(sphere);
}

void CreateCylinder(LPMESH* mesh, BRUSH* brush, int numSegments)
{
    LPSURFACE zylinder = NULL;

    const float height = 3.0f;
    const float radius = 1.0f;

    Engine::CreateMesh(mesh, brush);
    Engine::CreateSurface(&zylinder, (*mesh));

    float segmentAngle = (2 * 3.14f) / numSegments;

    // Erstelle die Vertices für die Mantelfläche
    for (int i = 0; i < numSegments; ++i) { 
        float angle = segmentAngle * i;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Untere Seite des Zylinders
        Engine::AddVertex(zylinder, x, -height / 2.0f, z);
        Engine::VertexNormal(zylinder, x, 0, z);
        Engine::VertexColor(zylinder, -1, 224, 224, 224);

        // Obere Seite des Zylinders
        Engine::AddVertex(zylinder, x, height / 2.0f, z);
        Engine::VertexNormal(zylinder, x, 0, z);
        Engine::VertexColor(zylinder, -1, 224, 224, 224);
    }

    // Erstelle die Vertices für die Deckfläche
    for (int i = 0; i < numSegments; ++i) {
        float angle = segmentAngle * i;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Untere Deckfläche des Zylinders
        Engine::AddVertex(zylinder, x, -height / 2.0f, z);
        Engine::VertexNormal(zylinder, 0, -1, 0);
        Engine::VertexColor(zylinder, 224, 224, 224);
    }
    // Untere Deckfläche des Zylinders
    Engine::AddVertex(zylinder, 0, -height / 2.0f, 0);
    Engine::VertexNormal(zylinder, 0, -1, 0);
    Engine::VertexColor(zylinder, 224, 224, 224);

    // Erstelle die Vertices für die Deckfläche
    for (int i = 0; i < numSegments; ++i) {
        float angle = segmentAngle * i;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Obere Deckfläche des Zylinders
        Engine::AddVertex(zylinder, x, height / 2.0f, z);
        Engine::VertexNormal(zylinder, 0, 1, 0);
        Engine::VertexColor(zylinder, 224, 224, 224);
    }
    // Untere Deckfläche des Zylinders
    Engine::AddVertex(zylinder, 0, height / 2.0f, 0);
    Engine::VertexNormal(zylinder, 0, 1, 0);
    Engine::VertexColor(zylinder, 224, 224, 224);

    // Verbinde die letzten Vertices mit den ersten im Uhrzeigersinn
    for (int i = 0; i < numSegments * 2; i += 2) {
        Engine::AddTriangle(zylinder, i, i + 1, (i + 2) % (numSegments * 2));
        Engine::AddTriangle(zylinder, (i + 2) % (numSegments * 2), i + 1, (i + 3) % (numSegments * 2));
    }

    // Deckel
    int startIndex = (numSegments * 2);
    for (int i = startIndex; i < startIndex + numSegments; i++) {
        Engine::AddTriangle(zylinder, i + 1, startIndex + numSegments,i);
    }
    Engine::AddTriangle(zylinder, startIndex, startIndex + numSegments, startIndex + numSegments-1);


    // Deckel
    startIndex = (numSegments * 3) ;
    for (int i = startIndex + numSegments; i > startIndex; i--) {
        Engine::AddTriangle(zylinder, i, startIndex + numSegments, i + 1);
    }

    Engine::FillBuffer(zylinder);
}



