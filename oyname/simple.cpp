#include "InputLayoutManager.h"
#include "gidx.h"

int main2()
{
    Engine::Graphics(1024, 768);

    Engine::Cls(0, 64, 128);

    while (gdx::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) // Main loop
    {
        if(FAILED(Engine::Flip()))
            Debug::Log("FEHLER");

    }

    // Shutdown the engine
    return gdx::ShutDown();
}

