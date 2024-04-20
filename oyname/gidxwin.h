#pragma once

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string> 

namespace Windows
{
    class CWindow
    {
    public:
        CWindow();

        ~CWindow();

        HWND Init(WNDPROC    wpCallback,
            HINSTANCE   hInst,
            LPCWSTR lpstrName,
            LPCWSTR lpstrTitle,
            WORD        wAppIcon);

        BOOL Release(void);

        HINSTANCE    GetInst(void);

        HWND         GetHwnd(void);

        unsigned int GetColorRes(void);

        void CreateConsole(void);

    private:
        bool        m_bInitialized;

        HINSTANCE   m_hInst;

        HWND        m_hWnd;

        int         m_nWidth;

        int         m_nHeight;

        std::wstring m_sName; 
    };
    typedef CWindow* LPWINDOWCLASS;
}
