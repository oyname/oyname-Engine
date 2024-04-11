#include "gidx.h"

// OYNAME Engine
// 
// A simple example demonstrating the basic usage of the oyname Engine
// It creates a DirectX window and clears the background with a predefined color in a loop.

int main_off2()
{
    Engine::Graphics(1024, 768);

    while (gdx::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) // Main loop
    {    
        Engine::Cls(0, 64, 128);

        if(FAILED(Engine::Flip()))
            Debug::Log("FEHLER");

        Engine::RenderWorld();
    }

    // Shutdown the engine
    return gdx::ShutDown();
}

