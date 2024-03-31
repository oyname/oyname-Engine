#include "gdxengine.h"


HRESULT Debug::GetErrorMessage(const char* file, int line, DWORD errorCode) {
	if (errorCode == ERROR_SUCCESS) {
		return S_OK;
	}

	LPSTR messageBuffer = NULL;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&messageBuffer,
		0,
		NULL);

	std::string errorMessage;

	if (size != 0) {
		errorMessage = messageBuffer;
		LocalFree(messageBuffer);
	}
	else {
		errorMessage = "Unknown error.";
	}

	Debug::Log(file," in line: ", line, " Error : ", errorMessage);

	return E_FAIL; // Return failure code if error occurred
}


namespace Engine
{
	gdx::CGIDX *engine;
}

//-----------------------------------------------------------------------------
// Name: gdx::CreateGidx() und gdx::Release()
// Desc: gidxLib functions are automatically called when the window is created. 
// Their use by end-users is not intended.
//-----------------------------------------------------------------------------
namespace gdx
{
	bool _running;

	int CreateGidx(int hwnd, int hinst, int bpp, int screenX, int screenY)
	{
		int result = 0;

		Engine::engine = nullptr;

		// Engine ertellen
		Engine::engine = new CGIDX((HWND)hwnd, (HINSTANCE)hinst, bpp, screenX, screenY, &result);
		if (Engine::engine == NULL)
			result = static_cast<int>(Error::CreateEngineFailed);//Memory request error

		// Variable befüllen
		_running = true;

		return result;
	}

	void Release()
	{
		Memory::SafeDelete(Engine::engine);
	}

	int ShutDown()
	{
		if(Engine::engine!=nullptr)
			PostMessage(Engine::engine->GetHWND(), WM_APP + 1, 0, 0);

		return 0;
	}

	void MainLoop(bool running)
	{
		_running = running;
	}
	
	bool MainLoop()
	{
		return _running;
	}
}

namespace Common
{
	inline GXFORMAT operator|(GXFORMAT lhs, GXFORMAT rhs)
	{
		return static_cast<GXFORMAT>(static_cast<int>(lhs) | static_cast<int>(rhs));
	}

	inline GXFORMAT& operator|=(GXFORMAT& lhs, GXFORMAT rhs)
	{
		lhs = static_cast<GXFORMAT>(static_cast<int>(lhs) | static_cast<int>(rhs));
		return lhs;
	}

	DXGI_FORMAT GetDXGIFormat(GXFORMAT format)
	{
		switch (format)
		{
		case GXFORMAT::B8G8R8A8_UNORM:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		case GXFORMAT::B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		case GXFORMAT::R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case GXFORMAT::R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case GXFORMAT::R16G16B16A16_FLOAT:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case GXFORMAT::R10G10B10A2_UNORM:
			return DXGI_FORMAT_R10G10B10A2_UNORM;
		case GXFORMAT::R10G10B10_XR_BIAS_A2_UNORM:
			return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
		default:
			// Handle unknown format
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	std::wstring ErrorToString(Error err) {
		switch (err) {
		case Error::Success:
			return L"Success";
		case Error::CreateEngineFailed:
			return L"Failed to create engine";
		case Error::CreateDeviceFailed:
			return L"Failed to create device";
		case Error::CreateInterfaceFailed:
			return L"Failed to create interface";
		case Error::CreateFactoryFailed:
			return L"Failed to create factory";
		case Error::CreateGIDXInputFailed:
			return L"Failed to create GIDX input";
		case Error::CreateInputFailed:
			return L"Failed to create input";
		case Error::CreateSoundFailed:
			return L"Failed to create sound";
		case Error::EnumAdapterFailed:
			return L"Failed to enumerate adapter";
		case Error::MemoryRequestDevice:
			return L"Memory request for device failed";
		case Error::InvalidParameter:
			return L"InvalidParameter";
		default:
			return L"Unknown error";
		}
	}

	std::wstring GetTextureFormatName(GXFORMAT format) {
		for (const auto& textureFormat : textureFormats) {
			if (textureFormat.format == format) {
				return textureFormat.name;
			}
		}
		return L"Unknown";
	}

	bool isHardwareSupported(IDXGIAdapter* pAdapter)
	{
		// Überprüfen, ob das Adapter-Objekt gültig ist
		if (!pAdapter) {
			return false;
		}

		// Überprüfen, ob der Adapter die gewünschte Hardwarefunktion unterstützt
		DXGI_ADAPTER_DESC adapterDesc;
		pAdapter->GetDesc(&adapterDesc);

		// Überprüfen, ob der Adapter eine dedizierte GPU hat
		return adapterDesc.DedicatedVideoMemory > 0;
	}

	bool isWindowsRenderer(IDXGIAdapter* adapter)
	{
		DXGI_ADAPTER_DESC desc;
		if (FAILED(adapter->GetDesc(&desc))) {
			// Fehler beim Abrufen der Adapterbeschreibung
			return false;
		}

		// Überprüfen, ob es sich nicht um den Microsoft Basic Render-Treiber handelt
		if ((desc.VendorId != 0x1414) || (desc.DeviceId != 0x8c)) {
			return false; // Es handelt sich nicht um den Microsoft Basic Render-Treiber (Windows-Renderer)
		}

		return true; // Es handelt sich um den Microsoft Basic Render-Treiber
	}

	GXFORMAT GetSupportedFormats(D3D_FEATURE_LEVEL featureLevel)
	{
		GXFORMAT supportedFormats = NONE;
		int version = 0;

		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_9_1:
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_3:
			supportedFormats |= B8G8R8A8_UNORM | B8G8R8A8_UNORM_SRGB |
				R8G8B8A8_UNORM | R8G8B8A8_UNORM_SRGB;
			break;
		case D3D_FEATURE_LEVEL_10_0:
			supportedFormats |= B8G8R8A8_UNORM | B8G8R8A8_UNORM_SRGB |
				R8G8B8A8_UNORM | R8G8B8A8_UNORM_SRGB |
				R16G16B16A16_FLOAT | R10G10B10A2_UNORM |
				R10G10B10_XR_BIAS_A2_UNORM;
			break;
		case D3D_FEATURE_LEVEL_10_1:
		case D3D_FEATURE_LEVEL_11_0:
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_12_0:
		case D3D_FEATURE_LEVEL_12_1:
			supportedFormats |= B8G8R8A8_UNORM | B8G8R8A8_UNORM_SRGB |
				R8G8B8A8_UNORM | R8G8B8A8_UNORM_SRGB |
				R16G16B16A16_FLOAT | R10G10B10A2_UNORM |
				R10G10B10_XR_BIAS_A2_UNORM;
			break;
		default:
			// Unbekanntes Feature-Level
			break;
		}
		return supportedFormats;
	}

	int GetDirectXVersion(D3D_FEATURE_LEVEL featureLevel)
	{
		int version = 0;

		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_9_1:
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_3:
			version = 1; // DirectX 9
			break;
		case D3D_FEATURE_LEVEL_10_0:
			version = 2; // DirectX 10
			break;
		case D3D_FEATURE_LEVEL_10_1:
			version = 3; // DirectX 10.1
			break;
		case D3D_FEATURE_LEVEL_11_0:
			version = 4; // DirectX 11
			break;
		case D3D_FEATURE_LEVEL_11_1:
			version = 5; // DirectX 11.1
			break;
		case D3D_FEATURE_LEVEL_12_0:
			version = 6; // DirectX 12
			break;
		case D3D_FEATURE_LEVEL_12_1:
			version = 7; // DirectX 12
			break;
		default:
			version = 0; // Unbekannte Version
			break;
		}
		return version;
	}

	D3D_FEATURE_LEVEL GetFeatureLevelFromDirectXVersion(int version)
	{
		D3D_FEATURE_LEVEL featureLevel;

		switch (version)
		{
		case 1:
			featureLevel = D3D_FEATURE_LEVEL_9_1; // DirectX 9.1
			break;
		case 2:
			featureLevel = D3D_FEATURE_LEVEL_10_0; // DirectX 10
			break;
		case 3:
			featureLevel = D3D_FEATURE_LEVEL_10_1; // DirectX 10.1
			break;
		case 4:
			featureLevel = D3D_FEATURE_LEVEL_11_0; // DirectX 11
			break;
		case 5:
			featureLevel = D3D_FEATURE_LEVEL_11_1; // DirectX 11.1
			break;
		case 6:
			featureLevel = D3D_FEATURE_LEVEL_12_0; // DirectX 12
			break;
		case 7:
			featureLevel = D3D_FEATURE_LEVEL_12_1; // DirectX 12.1
			break;
		default:
			featureLevel = D3D_FEATURE_LEVEL_9_1;  // Standardwert
			break;
		}
		return featureLevel;
	}
}// Common