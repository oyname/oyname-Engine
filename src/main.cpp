#include "main.h"
#include "gidxwin.h"

#include <objbase.h>   // CoInitialize / CoUninitialize
#include <thread>
#include <atomic>

// Link Ole32 for COM
#pragma comment(lib, "Ole32.lib")

INT APIENTRY WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    // COM init (needed if you use COM APIs anywhere; safe to keep)
    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr))
        return -1;

    MSG msg{};
    Windows::CWindow win;

    // 1) Create window (must happen before engine init)
    HWND hwnd = win.Init(WindowProc, hInst, L"gidx", L"", 101);
    if (!hwnd)
    {
        MessageBox(nullptr, L"Fatal: Window init failed.", L"ERROR", MB_OK);
        CoUninitialize();
        return 0;
    }

    // 2) Create engine
    const int bpp = (int)win.GetColorRes();
    const int w = GetSystemMetrics(SM_CXSCREEN);
    const int h = GetSystemMetrics(SM_CYSCREEN);

    int rc = Windows::CreateGidx(hwnd, hInst, bpp, w, h);
    if (rc != 0)
    {
        MessageBox(nullptr, L"Fatal: Engine init failed.", L"ERROR", MB_OK);
        Windows::Release();
        CoUninitialize();
        return 0;
    }

    // 3) Run engine/game loop on separate thread (your existing design)
    std::atomic_bool threadFinished{ false };
    std::thread mainThread([&]()
        {
            main(reinterpret_cast<LPVOID>(hwnd));
            threadFinished = true;
        });

    // 4) Message loop (main thread)
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 5) Ensure thread exits (your main() should stop when window closes)
    // If your main() doesnít check a flag yet, add one later.
    if (mainThread.joinable())
        mainThread.join();

    // 6) Cleanup
    Windows::Release();
    CoUninitialize();
    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;
        }
        break;

    case WM_CLOSE:
        Windows::MainLoop(false);

        // Fenster wirklich schlieﬂen
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        Windows::MainLoop(false);

        // Message-Loop beenden
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

