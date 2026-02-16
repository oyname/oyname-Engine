// game.cpp - Beispiel Game-Loop
//
// WICHTIG: Diese Funktion laeuft auf dem separaten Game-Thread.
// Core::Init() und Core::CreateEngine() werden bereits in WinMain (main.cpp) aufgerufen.
// Hier muss NUR die Spiellogik stehen.

#include <Windows.h>
#include "gidx.h"
#include "core.h"

LPENTITY g_plateMesh        = nullptr;
LPENTITY g_cubeMesh         = nullptr;
LPENTITY g_camera           = nullptr;
LPENTITY g_redLight         = nullptr;
LPENTITY g_blueLight        = nullptr;
LPENTITY g_directionalLight = nullptr;

void CreateCube(LPENTITY* mesh, MATERIAL* material);

// Signatur muss zu main.h passen: int main(LPVOID hwnd)
int main(LPVOID hwnd)
{
    // Engine ist schon initialisiert (WinMain hat Core::Init + Core::CreateEngine aufgerufen).
    // Hier nur noch Graphics-Modus setzen:
    Engine::Graphics(1024, 768);

    // Texturen
    LPTEXTURE brick = nullptr;
    Engine::LoadTexture(&brick, L"..\\media\\dx.bmp");

    LPTEXTURE dxlogo = nullptr;
    Engine::LoadTexture(&dxlogo, L"..\\media\\bricks.bmp");

    // Kamera
    Engine::CreateCamera(&g_camera);
    Engine::PositionEntity(g_camera, 0.0f, 40.0f, -90.0f);
    Engine::RotateEntity(g_camera, 25.0f, 0.0f, 0.0f);

    // Wuerfel
    LPMATERIAL whiteMaterial = nullptr;
    Engine::CreateMaterial(&whiteMaterial);
    Engine::MaterialTexture(whiteMaterial, brick);

    Engine::CreateMesh(&g_cubeMesh);
    CreateCube(&g_cubeMesh, whiteMaterial);
    Engine::PositionEntity(g_cubeMesh, 0.0f, 25.0f, 0.0f);
    Engine::ScaleEntity(g_cubeMesh, 5.0f, 5.0f, 5.0f);

    // Platte
    LPMATERIAL grayMaterial = nullptr;
    Engine::CreateMaterial(&grayMaterial);
    Engine::MaterialTexture(grayMaterial, dxlogo);

    Engine::CreateMesh(&g_plateMesh);
    CreateCube(&g_plateMesh, grayMaterial);
    Engine::PositionEntity(g_plateMesh, 0.0f, 0.0f, 0.0f);
    Engine::ScaleEntity(g_plateMesh, 50.0f, 0.5f, 50.0f);

    // Lichter
    Engine::CreateLight(&g_redLight, D3DLIGHT_POINT);
    Engine::PositionEntity(g_redLight, 20.0f, 15.0f, 0.0f);
    Engine::LightColor(g_redLight, 1.0f, 0.3f, 0.3f);

    Engine::CreateLight(&g_blueLight, D3DLIGHT_POINT);
    Engine::PositionEntity(g_blueLight, -20.0f, 15.0f, 0.0f);
    Engine::LightColor(g_blueLight, 0.3f, 0.3f, 1.0f);

    Engine::CreateLight(&g_directionalLight, D3DLIGHT_DIRECTIONAL);
    Engine::PositionEntity(g_directionalLight, 0.0f, 150.0f, 0.0f);
    Engine::RotateEntity(g_directionalLight, -90.0f, 0.0f, 0.0f);
    Engine::LightColor(g_directionalLight, 0.7f, 0.7f, 0.7f);

    // Dieses Licht wirft Schatten (muss nach CreateLight aufgerufen werden)
    Engine::SetDirectionalLight(g_directionalLight);

    Engine::SetVSync(1);

    const float speed = 100.0f;

    // === GAME LOOP ===
    while (Windows::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
    {
        Core::BeginFrame();

        const float dt = static_cast<float>(Core::GetDeltaTime());

        Engine::Cls(0, 64, 128);

        Engine::TurnEntity(g_cubeMesh, speed * dt, speed * dt, 0.0f);

        Engine::UpdateWorld();
        Engine::RenderWorld();
        Engine::Flip();

        Core::EndFrame();
    }

    // KEIN Engine::ReleaseEngine() hier!
    // KEIN Core::Shutdown() hier!
    // WinMain (main.cpp) ruft Core::Shutdown() auf, das macht beides automatisch.

    Debug::Log("game.cpp: MAIN BEENDET. Letzte FPS: ", Core::GetFPS());

    return 0;
}
