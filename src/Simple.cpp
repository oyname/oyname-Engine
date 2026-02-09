#include "gidx.h"

// OYNAME Engine
// 
// A simple example demonstrating the basic usage of the oyname Engine
// It creates a DirectX window and clears the background with a predefined color in a loop.

int main5()
{
    Engine::Graphics(1024, 768);

    // Creating camera object
    LPENTITY camera;
    Engine::CreateCamera(&camera);

    while (Windows::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) // Main loop
    {   
        // Clear Screen
        Engine::Cls(0, 64, 128);

        //
        Engine::UpdateWorld();

        //
        if(FAILED(Engine::RenderWorld()))
            Debug::Log("FEHLER");

        // Show
        if (FAILED(Engine::Flip()))
            Debug::Log("FEHLER");
    }
    
    // Shutdown the engine
    return Windows::ShutDown();
}

