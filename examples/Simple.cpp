// game.cpp
#include "gidx.h"
#include "core.h"

int main(LPVOID /*hwnd*/)
{
    // Engine ist schon initialisiert (WinMain hat Core::Init + Core::CreateEngine aufgerufen).
    // Hier nur noch Graphics-Modus setzen:
    Engine::Graphics(1024, 768);

    LPENTITY camera = nullptr;
    Engine::CreateCamera(&camera);

    while (Windows::MainLoop())
    {
        Core::BeginFrame(); // liefert DeltaTime/FPS/FrameCount über Core

        // Clear Screen (deine Engine erwartet floats 0..1 oder 0..255 je nach Implementierung)
        // In deinem Code nutzt du ints -> bleibt so, wenn Engine::Cls das so will:
        Engine::Cls(0, 64, 128);

        Engine::UpdateWorld();

        HRESULT hr = Engine::RenderWorld();
        if (FAILED(hr))
            Debug::Log("FEHLER: RenderWorld");

        hr = Engine::Flip();
        if (FAILED(hr))
            Debug::Log("FEHLER: Flip");

        Core::EndFrame();
    }

    return 0;
}