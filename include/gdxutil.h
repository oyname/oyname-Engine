#pragma once

#ifndef GDXUTIL_H_INCLUDED
#define GDXUTIL_H_INCLUDED

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <string>
#include <sstream>
#include <iostream>
#include <mutex>
#include <iomanip>
#include <chrono>

#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>

// ============================================================
// Enums / Flags
// ============================================================

enum D3DLIGHTTYPE {
    D3DLIGHT_POINT = 1,
    D3DLIGHT_SPOT = 2,
    D3DLIGHT_DIRECTIONAL = 3,
};

enum D3DVERTEX_FLAGS {
    D3DVERTEX_POSITION = (1 << 0),
    D3DVERTEX_COLOR = (1 << 1),
    D3DVERTEX_NORMAL = (1 << 2),
    D3DVERTEX_TEX1 = (1 << 3),
    D3DVERTEX_TEX2 = (1 << 4),
    D3DVERTEX_TEX3 = (1 << 5),
    D3DVERTEX_DIFFUSE = (1 << 6),
    D3DVERTEX_SPECULAR = (1 << 7),
};

enum GXFORMAT {
    FORMAT_NONE = 0,
    B8G8R8A8_UNORM = 1 << 0,
    B8G8R8A8_UNORM_SRGB = 1 << 1,
    R8G8B8A8_UNORM = 1 << 2,
    R8G8B8A8_UNORM_SRGB = 1 << 3,
    R16G16B16A16_FLOAT = 1 << 4,
    R10G10B10A2_UNORM = 1 << 5,
    R10G10B10_XR_BIAS_A2_UNORM = 1 << 6,
};

// Bitwise ops f�r GXFORMAT (GLOBAL, nicht in GXUTIL!)
inline GXFORMAT operator|(GXFORMAT a, GXFORMAT b)
{
    return static_cast<GXFORMAT>(static_cast<int>(a) | static_cast<int>(b));
}
inline GXFORMAT& operator|=(GXFORMAT& a, GXFORMAT b)
{
    a = a | b;
    return a;
}

// ============================================================
// Common structs
// ============================================================

__declspec(align(16))
struct MatrixSet
{
    DirectX::XMMATRIX viewMatrix;
    DirectX::XMMATRIX projectionMatrix;
    DirectX::XMMATRIX worldMatrix;
};

// ============================================================
// GXUTIL API (Implementierung in gdxutil.cpp)
// ============================================================

namespace GXUTIL
{
    DXGI_FORMAT   GetDXGIFormat(GXFORMAT format);
    std::wstring  GetTextureFormatName(GXFORMAT format);

    bool          isHardwareSupported(IDXGIAdapter* pAdapter);
    bool          isWindowsRenderer(IDXGIAdapter* adapter);

    GXFORMAT      GetSupportedFormats(D3D_FEATURE_LEVEL featureLevel);

    int           GetDirectXVersion(D3D_FEATURE_LEVEL featureLevel);
    D3D_FEATURE_LEVEL GetFeatureLevelFromDirectXVersion(int version);
    std::wstring  GetFeatureLevelName(D3D_FEATURE_LEVEL featureLevel);

    // -------- UTF helpers (header-only, kein <codecvt>) --------
    inline std::string WideToUtf8(const wchar_t* wstr)
    {
        if (!wstr) return {};
        int needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
        if (needed <= 0) return {};
        std::string out;
        out.resize(static_cast<size_t>(needed - 1));
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, out.data(), needed, nullptr, nullptr);
        return out;
    }

    inline std::wstring Utf8ToWide(const std::string& str)
    {
        if (str.empty()) return {};
        int needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        if (needed <= 0) return {};
        std::wstring out;
        out.resize(static_cast<size_t>(needed - 1));
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, out.data(), needed);
        return out;
    }
}

// ============================================================
// Debug (header-only) � getrennt HR / Win32, thread-safe
// ============================================================

class Debug
{
public:
    static inline bool s_outputDebugString = true;

    template<typename... Args>
    static void Log(Args&&... args) { Write("[LOG] ", std::cout, std::forward<Args>(args)...); }

    template<typename... Args>
    static void LogWarning(Args&&... args) { Write("[WARNING] ", std::cout, std::forward<Args>(args)...); }

    template<typename... Args>
    static void LogError(Args&&... args) { Write("[ERROR] ", std::cerr, std::forward<Args>(args)...); }

    static void LogHr(const char* file, int line, HRESULT hr)
    {
        if (SUCCEEDED(hr)) return;
        LogError(file, ":", line, " -> ", FormatWin32Message((DWORD)hr),
            " (HRESULT=0x", std::hex, std::uppercase, (DWORD)hr, ")");
    }

    static void LogWin32(const char* file, int line)
    {
        DWORD err = GetLastError();
        if (err == 0) return;
        LogError(file, ":", line, " -> ", FormatWin32Message(err),
            " (Win32=0x", std::hex, std::uppercase, err, ")");
    }

private:
    inline static std::mutex s_mutex;

    static std::string Now()
    {
        using namespace std::chrono;
        const auto now = system_clock::now();
        const auto t = system_clock::to_time_t(now);

        std::tm tm{};
        localtime_s(&tm, &t);

        const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

        std::ostringstream oss;
        oss << std::setfill('0')
            << std::setw(2) << tm.tm_hour << ":"
            << std::setw(2) << tm.tm_min << ":"
            << std::setw(2) << tm.tm_sec << "."
            << std::setw(3) << ms.count();
        return oss.str();
    }

    static std::string FormatWin32Message(DWORD code)
    {
        if (code == 0) return "OK";

        LPSTR buf = nullptr;
        DWORD len = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&buf, 0, nullptr);

        std::string msg = "Unknown error";
        if (len && buf)
        {
            msg.assign(buf, buf + len);
            while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r')) msg.pop_back();
        }
        if (buf) LocalFree(buf);
        return msg;
    }

    template<typename T>
    static void Append(std::ostringstream& oss, const T& t) { oss << t; }

    static void Append(std::ostringstream& oss, const wchar_t* w) { oss << GXUTIL::WideToUtf8(w); }
    static void Append(std::ostringstream& oss, const std::wstring& w) { oss << GXUTIL::WideToUtf8(w.c_str()); }

    template<typename T, typename... Args>
    static void AppendAll(std::ostringstream& oss, const T& t, Args&&... args)
    {
        Append(oss, t);
        if constexpr (sizeof...(args) > 0) AppendAll(oss, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Write(const char* prefix, std::ostream& stream, Args&&... args)
    {
        std::ostringstream oss;
        oss << Now() << " " << prefix;
        AppendAll(oss, std::forward<Args>(args)...);

        const std::string line = oss.str();

        {
            std::lock_guard<std::mutex> lock(s_mutex);
            stream << line << "\n";
            stream.flush();
        }
        if (s_outputDebugString)
            OutputDebugStringA((line + "\n").c_str());
    }
};

#define GDX_HR(x)   do { HRESULT _hr=(x); Debug::LogHr(__FILE__, __LINE__, _hr); } while(0)
#define GDX_WIN32() do { Debug::LogWin32(__FILE__, __LINE__); } while(0)

// ============================================================
// Memory helpers
// ============================================================

namespace Memory
{
    template<typename T>
    inline void SafeRelease(T*& p)
    {
        if (p) { p->Release(); p = nullptr; }
    }

    template<typename T>
    inline void SafeDelete(T*& p)
    {
        if (p) { delete p; p = nullptr; }
    }

    template<typename T>
    inline void SafeDeleteArray(T*& p)
    {
        if (p) { delete[] p; p = nullptr; }
    }
}

#endif // GDXUTIL_H_INCLUDED
