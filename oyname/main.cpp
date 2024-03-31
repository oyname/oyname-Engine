#include"main.h"

INT APIENTRY WinMain(HINSTANCE hInst,
	HINSTANCE hPrevInst,
	LPSTR pCmdLine,
	int nCmdShow)
{
	HWND hwnd;
	MSG msg;

	Windows::CWindow win;

	hwnd = win.Init(WindowProc, hInst, L"gidx", L"", 101);

	if (hwnd == NULL)
	{
		MessageBox(NULL, L" A fatal error occurred while initializing giDX². (GIDX_WIN) ", L"ERROR!", MB_OK);
		return 0;
	}

	gdx::CreateGidx(reinterpret_cast<int>(hwnd), reinterpret_cast<int>(hInst), win.GetColorRes(), GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) );

	// Thread mit std::thread erstellen
	std::thread mainThread(main, reinterpret_cast<LPVOID>(hwnd));

	if (!mainThread.joinable())
	{
		MessageBox(NULL, L" Error starting the Main-Thread ", L"ERROR!", MB_OK);
		return 1;
	}

	// Nachrichtenschleife
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Auf das Beenden des Threads warten
	mainThread.join();

	gdx::Release();

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			
			break;

		default:
			break;
		}
		break;
	case WM_CREATE:
		gdx::SetWindowBackgroundColor(0, 0, 0, hWnd);

		break;
	case WM_APP + 1:
		Sleep(100);
		PostMessage(hWnd, WM_QUIT, 0, 0);
		
		break;
	case WM_DESTROY:
	case WM_CLOSE:
	case WM_QUIT:
		gdx::MainLoop(false);

		return 0;
	default:
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

// Define the function with clear parameter names
void gdx::SetWindowBackgroundColor(int red, int green, int blue, HWND windowHandle) {
	// Create a solid brush with the specified color
	HBRUSH backgroundBrush = CreateSolidBrush(RGB(red, green, blue));

	// Ensure the brush creation was successful
	if (backgroundBrush != NULL) {
		// Set the background brush for the window class
		SetClassLongPtr(windowHandle, GCLP_HBRBACKGROUND, (LONG_PTR)backgroundBrush);

		// Invalidate the entire window area to trigger a redraw
		InvalidateRect(windowHandle, NULL, TRUE);
	}
}
