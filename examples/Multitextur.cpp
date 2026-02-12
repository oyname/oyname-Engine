#include "gidx.h"

int main5()
{
    bool windowed = true;

    windowed == true ? Engine::Graphics(1200, 650) : Engine::Graphics(1980, 1080, false);

    while (Windows::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) 
    {

    }

    return Windows::ShutDown();
}