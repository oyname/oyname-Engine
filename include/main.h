#pragma once
#include <windows.h>

namespace Windows
{
    int  CreateGidx(HWND hwnd, HINSTANCE hInst, int bpp, int width, int height);
    void Release();

    void MainLoop(bool running);
    bool MainLoop();

    int  ShutDown();
}

// Your window proc + your engine/game thread entry
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int main(LPVOID hwnd);

