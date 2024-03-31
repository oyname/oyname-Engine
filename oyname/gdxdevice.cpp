#include "gdxutil.h"
#include "gdxdevice.h"

using namespace gdx;

CDevice::CDevice() : m_bInitialized(false), 
                     m_pd3dDevice(nullptr), 
                     m_pContext(nullptr), 
                     m_pSwapChain(nullptr), 
                     m_depthStencilBuffer(nullptr), 
                     m_depthStencilView(nullptr),
                     m_pBackBuffer(nullptr),
                     m_pRenderTargetView(nullptr)
{                                                                                                              

}

CDevice::~CDevice()
{
	Release();
}

void CDevice::Release()
{
    if (m_bInitialized)
    {
        if (m_pSwapChain != nullptr)
            m_pSwapChain->SetFullscreenState(FALSE, NULL);

        Memory::SafeRelease(m_depthStencilBuffer);
        Memory::SafeRelease(m_depthStencilView);
        Memory::SafeRelease(m_pBackBuffer);
        Memory::SafeRelease(m_pSwapChain);
        Memory::SafeRelease(m_pContext);
        Memory::SafeRelease(m_pd3dDevice);
    }

    m_bInitialized = false;
}

HRESULT CDevice::Init()
{
    return GetSystemInfo();
}

HRESULT CDevice::GetSystemInfo()
{
    HRESULT hr;

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    ID3D11Device* d3ddevice = nullptr;
    IDXGIFactory* factory = nullptr;
    IDXGIAdapter* adapter = nullptr;

    hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory));
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        return hr;
    }

    D3D_FEATURE_LEVEL featureLevel;
    for (UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        HRESULT hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &d3ddevice, &featureLevel, nullptr);
        if (SUCCEEDED(hr))
        {
            if (!Common::isWindowsRenderer(adapter))
            {
                // Nur wenn kein Windows Renderer 
                GXDEVICE gxDevice = {};
                gxDevice.featureLevel = featureLevel;
                gxDevice.supportedFormat = Common::GetSupportedFormats(featureLevel);
                gxDevice.directxVersion = Common::GetDirectXVersion(gxDevice.featureLevel);
                
                // neues Device hinzufügen
                this->deviceManager.GetDevices().push_back(gxDevice);
            }
        }

        Memory::SafeRelease(adapter);

        return hr;
    }

    Memory::SafeRelease(d3ddevice);
    Memory::SafeRelease(factory);

    m_bInitialized = true;

    return hr;
}

HRESULT CDevice::InitializeDirectX(IDXGIAdapter* adapter, D3D_FEATURE_LEVEL* featureLevel)
{
    HRESULT hr = S_OK;

    // Create the Direct3D device
    hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, featureLevel, 1, D3D11_SDK_VERSION, &m_pd3dDevice, nullptr, &m_pContext);

    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        return hr;
    }

    return hr;
}

HRESULT CDevice::CreateSwapChain(IDXGIFactory* pDXGIFactory, HWND hWnd,
                                         unsigned int width, unsigned int height, DXGI_FORMAT format,
                                         unsigned int numerator, unsigned int denominator,
                                         bool windowed)
{
    HRESULT hr;

    // Fenstergröße anpassen
    ResizeWindow(hWnd, width, height, windowed);

    // Beschreibung des Swap Chain
    DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = (format != DXGI_FORMAT_UNKNOWN) ? format : DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = (numerator != 0) ? numerator : 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = (denominator != 0) ? denominator : 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = true; //windowed; 
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Vollbildumschaltung zulassen

    // Swap Chain erstellen
    hr = pDXGIFactory->CreateSwapChain(m_pd3dDevice, &swapChainDesc, &m_pSwapChain);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        // Freigabe des Speichers
        Memory::SafeRelease(m_pSwapChain);

        // Fehler zurückgeben
        return hr;
    }

    return hr;
}

HRESULT CDevice::CreateRenderTarget(unsigned int width, unsigned int height)
{
    HRESULT hr = S_OK;

    // Back-Buffer aus dem Swap-Chain abrufen
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_pBackBuffer);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        return hr;
    }
    else if(m_pBackBuffer)
    {
        hr = m_pd3dDevice->CreateRenderTargetView(m_pBackBuffer, NULL, &m_pRenderTargetView);
        if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
        {
            Memory::SafeRelease(m_pBackBuffer);
            return hr;
        }
        Memory::SafeRelease(m_pBackBuffer);
    }

    // Erfolgreich erstellten Render Target zurückgeben
    return hr;
}

void CDevice::CreateView(UINT NumViewport, D3D11_VIEWPORT viewport)
{
    m_pContext->RSSetViewports(NumViewport, &viewport);
}

void CDevice::ClearRenderTargetDepthStencil()
{
    // Setze die Render-Zielansicht und den Tiefen-Stencil-Buffer
    m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_depthStencilView);

    // Lösche den Tiefen-Stencil-Buffer
    m_pContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void gdx::CDevice::SetRenderTargets(unsigned int numViews)
{
    m_pContext->OMSetRenderTargets(numViews, &m_pRenderTargetView, NULL);
}

void CDevice::SetVertexShader(ID3D11VertexShader* vs)
{
    m_pContext->VSSetShader(vs, nullptr, 0);
}

void CDevice::SetPixelShader(ID3D11PixelShader* ps)
{
    m_pContext->PSSetShader(ps, nullptr, 0);
}

HRESULT CDevice::Flip(int syncInterval)
{
    // SyncInterval: Dieser Parameter legt fest, wie die Aktualisierungsrate des Monitors(die vertikale Synchronisation) 
    // verwendet wird.Ein Wert von 0 bedeutet, dass das Zurücksetzen auf die VSYNC nicht berücksichtigt wird, und das Bild 
    // wird sofort angezeigt, wenn es verfügbar ist.Ein Wert größer als 0 bedeutet, dass das Zurücksetzen auf die VSYNC alle 
    // SyncInterval - Frames erfolgt.Zum Beispiel bedeutet ein Wert von 1, dass das Zurücksetzen auf die VSYNC einmal alle 
    // zwei Frames erfolgt, was einer Aktualisierungsrate von 30 Hz auf einem 60 Hz Monitor entspricht.
    // //
    // Present the backbuffer to the screen
    return m_pSwapChain->Present(syncInterval, 0);
}

void CDevice::ResizeWindow(HWND hwnd, unsigned int x, unsigned int y, bool windowed)
{
    RECT rc;

    SetRect(&rc, 0, 0, x, y);

    if (windowed)
    {
        AdjustWindowRect(&rc, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_OVERLAPPED, false);
        SetWindowLong(hwnd, GWL_STYLE, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_OVERLAPPED);
    }
    else
    {
        SetWindowLong(hwnd, GWL_STYLE, WS_POPUP);
    }

    MoveWindow(hwnd,
        GetSystemMetrics(SM_CXSCREEN) / 2 - x / 2,
        GetSystemMetrics(SM_CYSCREEN) / 2 - y / 2,
        (rc.right - rc.left), (rc.bottom - rc.top), true);

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
}

HRESULT CDevice::CreateDeepBuffer(unsigned int width, unsigned int height)
{
    HRESULT hr = S_OK;

    // Create the texture for the depth buffer.
    D3D11_TEXTURE2D_DESC depthBufferDesc;

    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
    depthBufferDesc.Width = width;
    depthBufferDesc.Height = height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    hr = GetDevice()->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        return hr;
    }

    // Create the depth stencil view.
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    hr = GetDevice()->CreateDepthStencilView(m_depthStencilBuffer,
        &depthStencilViewDesc,
        &m_depthStencilView);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        return hr;
    }

    // Setze die Tiefen-Stencil-Ansicht
    GetDeviceContext()->OMSetRenderTargets(1, &m_pRenderTargetView, m_depthStencilView);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        return hr;
    }
    
    // Definiere den Tiefen-Stencil-Status
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = true; // Tiefentest aktivieren
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // Schreiben in den Tiefenpuffer erlauben
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Vergleichsoperation für den Tiefentest
    // Weitere Einstellungen hier definieren
    
    // Erstelle den Tiefen-Stencil-Status
    ID3D11DepthStencilState* depthStencilState;
    hr = GetDevice()->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        return hr;
    }
    
    // Setze den Tiefen-Stencil-Status
    GetDeviceContext()->OMSetDepthStencilState(depthStencilState, 1);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        return hr;
    }
    
    // Freigabe des Depth-Stencil-State
    Memory::SafeRelease(depthStencilState);

    return hr;
}