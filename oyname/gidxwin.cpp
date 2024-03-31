#include "gidxwin.h"
#include "gdxutil.h"

namespace Windows
{
    CWindow::CWindow() :
        m_hInst(nullptr),
        m_hWnd(nullptr),
        m_nWidth(500),
        m_nHeight(500),
        m_bInitialized(false),
        m_sName(L"")
    {
    }

    CWindow::~CWindow()
    {
        Release();
    }

    HWND CWindow::Init(WNDPROC wpCallback,
        HINSTANCE hInst,
        LPCWSTR lpstrName,
        LPCWSTR lpstrTitle,
        WORD AppIcon)
    {
        if (m_bInitialized)
            return m_hWnd;

        WNDCLASSEX winclass = { sizeof(WNDCLASSEX) };

        winclass.style = CS_HREDRAW | CS_VREDRAW;
        winclass.lpfnWndProc = wpCallback;
        winclass.hInstance = hInst;
        winclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        winclass.lpszClassName = lpstrName;
        winclass.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(AppIcon));
        winclass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

        if (!RegisterClassEx(&winclass))
            return nullptr;

        RECT wr = { 0, 0, m_nWidth, m_nHeight };
        AdjustWindowRect(&wr, WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

        m_hWnd = CreateWindowEx(NULL,
            lpstrName,
            lpstrTitle,
            WS_SYSMENU | WS_MINIMIZEBOX,
            GetSystemMetrics(SM_CXSCREEN) / 2 - m_nWidth / 2,
            GetSystemMetrics(SM_CYSCREEN) / 2 - m_nHeight / 2,
            m_nWidth,
            m_nHeight,
            nullptr,
            nullptr,
            hInst,
            nullptr);

        if (m_hWnd == nullptr)
            return nullptr;

        CreateConsole();

        ShowCursor(TRUE);
        ShowWindow(m_hWnd, SW_SHOW);
        UpdateWindow(m_hWnd);
        SetFocus(m_hWnd);

        m_hInst = hInst;
        m_sName = lpstrName;
        m_bInitialized = true;

        return m_hWnd;
    }

    BOOL CWindow::Release()
    {
        if (m_hWnd != nullptr)
            m_hWnd = nullptr;

        m_bInitialized = false;

        ShowCursor(TRUE);
        return UnregisterClass(m_sName.c_str(), m_hInst);
    }

    HWND CWindow::GetHwnd()
    {
        return m_hWnd;
    }

    HINSTANCE CWindow::GetInst()
    {
        return m_hInst;
    }

    unsigned int CWindow::GetColorRes()
    {
        HDC hdc = GetDC(GetHwnd());
        unsigned int res = GetDeviceCaps(hdc, BITSPIXEL);
        ReleaseDC(GetHwnd(), hdc);
        return res;
    }

    void CWindow::CreateConsole()
    {
        AllocConsole();
        SetConsoleTitle(L"Engine Console...");

        FILE* new_stdout;
        freopen_s(&new_stdout, "CONOUT$", "w", stdout);

        HWND consoleWindow = GetConsoleWindow();
        RECT desktopRect;
        GetWindowRect(GetDesktopWindow(), &desktopRect);

        int consoleWidth = desktopRect.right;
        int consoleHeight = 230;
        int consoleX = 0;
        int consoleY = desktopRect.bottom - consoleHeight;

        SetWindowPos(consoleWindow, HWND_TOP, consoleX, consoleY, consoleWidth, consoleHeight, SWP_SHOWWINDOW);
    }
}
