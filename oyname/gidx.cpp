#include "gdxengine.h"

// Function to retrieve error message
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

    Debug::Log(file, " in line: ", line, " Error : ", errorMessage);

    return E_FAIL; // Return failure code if error occurred
}

// Engine namespace
namespace Engine {
    gdx::CGIDX* engine;
}

// Functions automatically called when window is created, intended for internal use
namespace gdx {
    bool _running;

    // Create GIDX function
    int CreateGidx(int hwnd, int hinst, int bpp, int screenX, int screenY) {
        int result = 0;

        Engine::engine = nullptr;

        // Create Engine
        Engine::engine = new CGIDX((HWND)hwnd, (HINSTANCE)hinst, bpp, screenX, screenY, &result);
        if (Engine::engine == NULL)
            result = static_cast<int>(Error::CreateEngineFailed); // Memory request error

        // Set variable
        _running = true;

        return result;
    }

    // Release function
    void Release() {
        Memory::SafeDelete(Engine::engine);
    }

    // Shutdown function
    int ShutDown() {
        if (Engine::engine != nullptr)
            PostMessage(Engine::engine->GetHWND(), WM_APP + 1, 0, 0);

        return 0;
    }

    // Main loop function
    void MainLoop(bool running) {
        _running = running;
    }

    // Main loop function
    bool MainLoop() {
        return _running;
    }
}

// GXUTIL namespace
namespace GXUTIL {
    // Bitwise OR operator for GXFORMAT
    inline GXFORMAT operator|(GXFORMAT lhs, GXFORMAT rhs) {
        return static_cast<GXFORMAT>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    // Compound assignment bitwise OR operator for GXFORMAT
    inline GXFORMAT& operator|=(GXFORMAT& lhs, GXFORMAT rhs) {
        lhs = static_cast<GXFORMAT>(static_cast<int>(lhs) | static_cast<int>(rhs));
        return lhs;
    }

    // Function to map GXFORMAT to DXGI_FORMAT
    DXGI_FORMAT GetDXGIFormat(GXFORMAT format) {
        switch (format) {
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

    // Function to convert error enum to string
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

    // Function to get texture format name
    std::wstring GetTextureFormatName(GXFORMAT format) {
        for (const auto& textureFormat : textureFormats) {
            if (textureFormat.format == format) {
                return textureFormat.name;
            }
        }
        return L"Unknown";
    }

    // Function to check if hardware is supported
    bool isHardwareSupported(IDXGIAdapter* pAdapter) {
        // Check if the adapter object is valid
        if (!pAdapter) {
            return false;
        }

        // Check if the adapter supports the desired hardware feature
        DXGI_ADAPTER_DESC adapterDesc;
        pAdapter->GetDesc(&adapterDesc);

        // Check if the adapter has a dedicated GPU
        return adapterDesc.DedicatedVideoMemory > 0;
    }

    // Function to check if using Windows renderer
    bool isWindowsRenderer(IDXGIAdapter* adapter) {
        DXGI_ADAPTER_DESC desc;
        if (FAILED(adapter->GetDesc(&desc))) {
            // Error getting adapter description
            return false;
        }

        // Check if it's not the Microsoft Basic Render Driver
        if ((desc.VendorId != 0x1414) || (desc.DeviceId != 0x8c)) {
            return false; // Not Microsoft Basic Render Driver (Windows Renderer)
        }

        return true; // It's Microsoft Basic Render Driver
    }

    // Function to get supported formats based on feature level
    GXFORMAT GetSupportedFormats(D3D_FEATURE_LEVEL featureLevel) {
        GXFORMAT supportedFormats = NONE;
        int version = 0;

        switch (featureLevel) {
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
            // Unknown feature level
            break;
        }
        return supportedFormats;
    }

    // Function to get DirectX version from feature level
    int GetDirectXVersion(D3D_FEATURE_LEVEL featureLevel) {
        int version = 0;

        switch (featureLevel) {
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
            version = 7; // DirectX 12.1
            break;
        default:
            version = 0; // Unknown version
            break;
        }
        return version;
    }

    // Function to get feature level from DirectX version
    D3D_FEATURE_LEVEL GetFeatureLevelFromDirectXVersion(int version) {
        D3D_FEATURE_LEVEL featureLevel;

        switch (version) {
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
            featureLevel = D3D_FEATURE_LEVEL_9_1; // Default
            break;
        }
        return featureLevel;
    }
} // namespace GXUTIL
