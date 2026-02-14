// gidx.cpp
#include "main.h"
#include "gdxengine.h"

namespace Engine
{
    gdx::CGIDX* engine = nullptr;
}

namespace Windows
{
    static bool g_running = true;

    void MainLoop(bool running) { g_running = running; }
    bool MainLoop() { return g_running; }

    int ShutDown() { g_running = false; return 0; }

    int CreateGidx(HWND hwnd, HINSTANCE hInst, int bpp, int width, int height)
    {
        int result = 0;
        Engine::engine = new gdx::CGIDX(hwnd, hInst,
            (unsigned)bpp,
            (unsigned)width,
            (unsigned)height,
            &result);
        return result;
    }

    void Release()
    {
        delete Engine::engine;
        Engine::engine = nullptr;
    }
}
