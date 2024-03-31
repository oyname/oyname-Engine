#ifndef GXUTIL_H
#define GXUTIL_H

#include <d3d11.h>
#include <dxgi.h>

#include <sstream>
#include <vector>
#include <memory>
#include <array>
#include <iostream>
#include <string>
#include <math.h>
#include <limits>
#include <list>
#include <functional>
#include <codecvt>
#include <DirectXMath.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3dcompiler.lib")

std::string GetErrorMessage(DWORD errorCode);

//
struct ConstantBuffer
{
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
	DirectX::XMMATRIX worldMatrix;
};

// 
enum D3DVERTEX_FLAGS
{
	D3DVERTEX_POSITION = (1 << 0),
	D3DVERTEX_COLOR = (1 << 1),
	D3DVERTEX_NORMAL = (1 << 2),
	D3DVERTEX_TEX1 = (1 << 3),
	D3DVERTEX_TEX2 = (1 << 4),
	D3DVERTEX_TEX3 = (1 << 5),
	D3DVERTEX_DIFFUSE = (1 << 6),
	D3DVERTEX_SPECULAR = (1 << 7),
	// Weitere Flags können hier hinzugefügt werden...
};

//
typedef enum 
{
	AUTOMATIC_WINDOW_MODE = 0,
	FULLSCREEN_MODE = 1,
	WINDOW_MODE = 2,
	SCALABLE_WINDOW_MODE = 3,
} GXDISPLAYMODE;

enum Options {
	WindowsRenderer = 1 << 0,
	HardwareSupport = 1 << 1,
	OPTION_3 = 1 << 2,
	// Add more options here ...
};

enum GXFORMAT
{
	NONE = 0,
	B8G8R8A8_UNORM = 1 << 0,
	B8G8R8A8_UNORM_SRGB = 1 << 1,
	R8G8B8A8_UNORM = 1 << 2,
	R8G8B8A8_UNORM_SRGB = 1 << 3,
	R16G16B16A16_FLOAT = 1 << 4,
	R10G10B10A2_UNORM = 1 << 5,
	R10G10B10_XR_BIAS_A2_UNORM = 1 << 6,
};

struct TextureFormat
{
	GXFORMAT format;
	LPCWSTR name;
};

const TextureFormat textureFormats[] = {
	{ GXFORMAT::B8G8R8A8_UNORM, L"B8G8R8A8_UNORM" },
	{ GXFORMAT::B8G8R8A8_UNORM_SRGB, L"B8G8R8A8_UNORM_SRGB" },
	{ GXFORMAT::R8G8B8A8_UNORM, L"R8G8B8A8_UNORM" },
	{ GXFORMAT::R8G8B8A8_UNORM_SRGB, L"R8G8B8A8_UNORM_SRGB" },
	{ GXFORMAT::R16G16B16A16_FLOAT, L"R16G16B16A16_FLOAT" },
	{ GXFORMAT::R10G10B10A2_UNORM, L"R10G10B10A2_UNORM" },
	{ GXFORMAT::R10G10B10_XR_BIAS_A2_UNORM, L"R10G10B10_XR_BIAS_A2_UNORM" },
};

enum class Error
{
	None,
	Success,
	CreateEngineFailed,
	CreateDeviceFailed,
	CreateInterfaceFailed,
	CreateSwapChainFailed,
	CreateFactoryFailed,
	CreateGIDXInputFailed,
	CreateInputFailed,
	CreateSoundFailed,
	CreateBackBuffer,
	InvalidDevice,
	InvalidSwapChain,
	EnumAdapterFailed,
	MemoryRequestDevice,
	WindowosRenderer,
	InvalidParameter 
};

namespace Common
{
	DXGI_FORMAT GetDXGIFormat(GXFORMAT format);
	std::wstring ErrorToString(Error err);
	std::wstring GetTextureFormatName(GXFORMAT format);
	bool isHardwareSupported(IDXGIAdapter* pAdapter);
	bool isWindowsRenderer(IDXGIAdapter* adapter);
	int GetDirectXVersion(D3D_FEATURE_LEVEL featureLevel);
	D3D_FEATURE_LEVEL GetFeatureLevelFromDirectXVersion(int version);
	GXFORMAT GetSupportedFormats(D3D_FEATURE_LEVEL featureLevel);

	struct SwapchainConfig
	{
		UINT bufferCount;
		UINT width;
		UINT height;
		DXGI_FORMAT bufferFormat;
		DXGI_RATIONAL refreshRate;
		DXGI_USAGE bufferUsage;
		HWND outputWindow;
		UINT sampleCount;
		UINT sampleQuality;
		BOOL windowed;
		UINT flags;

		// Konstruktor, um Standardwerte zu setzen
		SwapchainConfig() :
			bufferCount(2),                               // Anzahl der Back-Buffer
			width(800),                                   // Breite des Anzeigefensters oder der Bildschirmfläche
			height(600),                                  // Höhe des Anzeigefensters oder der Bildschirmfläche
			bufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM),     // Format des Back-Buffer
			refreshRate({ 0, 0 }),                        // Verwende die vom System vorgeschlagene Bildwiederholfrequenz
			bufferUsage(DXGI_USAGE_RENDER_TARGET_OUTPUT), // Verwendung des Back-Buffer
			outputWindow(NULL),                           // Handle des Ausgabefensters
			sampleCount(1),                               // Anzahl der Samples im Multisampling
			sampleQuality(0),                             // Qualität des Multisampling
			windowed(TRUE),                               // Fenstermodus (TRUE) oder Vollbildmodus (FALSE)
			flags(0)                                      // Optionen für die Swapchain
		{}
	};
}

class Debug
{
public:
	static HRESULT GetErrorMessage(const char* file, int line, DWORD errorCode);

	template<typename... Args>
	static void Log(Args&&... args)
	{
		std::ostringstream oss;
		LogHelper(oss, std::forward<Args>(args)...);
		std::cout << "[LOG] " << oss.str() << std::endl;
	}

	template<typename... Args>
	static void LogError(Args&&... args)
	{
		std::ostringstream oss;
		LogHelper(oss, std::forward<Args>(args)...);
		std::cout << "[ERROR] " << oss.str() << std::endl;
	}

	template<typename... Args>
	static void LogWarning(Args&&... args)
	{
		std::ostringstream oss;
		LogHelper(oss, std::forward<Args>(args)...);
		std::cout << "[WARNING] " << oss.str() << std::endl;
	}

private:
	static void LogHelper(std::ostringstream& oss, const wchar_t* str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		oss << converter.to_bytes(str);
	}

	template<typename T>
	static void LogHelper(std::ostringstream& oss, const T& t)
	{
		oss << t;
	}

	template<typename T, typename... Args>
	static void LogHelper(std::ostringstream& oss, const T& t, Args&&... args)
	{
		LogHelper(oss, t);
		LogHelper(oss, std::forward<Args>(args)...);
	}
};

namespace Memory
{
	// Memory Release
	//
	template<typename T>void SafeRelease(T*& IUnk)
	{
		if (IUnk != nullptr)
		{
			IUnk->Release();
			IUnk = nullptr;
		}
	}

	template<typename T> void SafeDeleteArray(T& arr)
	{
		if (arr)
		{
			delete[](arr);
			arr = 0;
		}
	}

	template<typename T> void SafeDelete(T& p)
	{
		if (p)
		{
			delete(p);
			p = nullptr;
		}
	}

	template<typename T> void DeleteBuffer(T& p)
	{
		unsigned int i;
		for (i = 0; i < p.size(); i++)
			SafeRelease(p[i]);

		p.clear();
	}

	template<typename T> void Erease(T& p)
	{
		unsigned int i;
		for (i = 0; i < p.size(); i++)
			p[i] = 0;

		p.clear();
	}
};

#endif // GXUTIL_H