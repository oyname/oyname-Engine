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
                     m_pRenderTargetView(nullptr),
                     m_pShadowRenderState(nullptr),
                     m_pComparisonSampler_point(nullptr),
                     m_depthStencilState(nullptr),
                     m_pRasterizerState(nullptr),
                     m_pShadowMap(nullptr),
                     m_pShadowMapSRView(nullptr),
                     m_pShadowMapDepthView(nullptr),
                     m_pShadowTargetView(nullptr)
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

        Memory::SafeRelease(m_pd3dDevice);
        Memory::SafeRelease(m_pContext);
        Memory::SafeRelease(m_pSwapChain);
        Memory::SafeRelease(m_pBackBuffer);
        Memory::SafeRelease(m_depthStencilBuffer);
        Memory::SafeRelease(m_depthStencilState);
        Memory::SafeRelease(m_depthStencilView);
        Memory::SafeRelease(m_pRenderTargetView);
        Memory::SafeRelease(m_pShadowMap);
        Memory::SafeRelease(m_pShadowMapSRView);
        Memory::SafeRelease(m_pShadowMapDepthView);
    }

    m_bInitialized = false;
}

HRESULT CDevice::Init()
{
    return GetSystemInfo();
}

HRESULT CDevice::GetSystemInfo()
{
    HRESULT hr = S_OK;

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
    if (SUCCEEDED(hr))
    {
        D3D_FEATURE_LEVEL featureLevel;
        for (UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &d3ddevice, &featureLevel, nullptr);
            if (SUCCEEDED(hr))
            {
                if (!GXUTIL::isWindowsRenderer(adapter))
                {
                    // Nur wenn kein Windows-Renderer verwendet wird
                    GXDEVICE gxDevice = {};
                    gxDevice.featureLevel = featureLevel;
                    gxDevice.supportedFormat = GXUTIL::GetSupportedFormats(featureLevel);
                    gxDevice.directxVersion = GXUTIL::GetDirectXVersion(gxDevice.featureLevel);

                    // Füge neues Gerät hinzu
                    this->deviceManager.GetDevices().push_back(gxDevice);
                }
            }
            else
                FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr));

            Memory::SafeRelease(adapter);
        }
    }

    Memory::SafeRelease(d3ddevice);
    Memory::SafeRelease(factory);

    m_bInitialized = SUCCEEDED(hr);

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
    HRESULT hr = S_OK;

    // Fenstergröße anpassen
    ResizeWindow(hWnd, width, height, windowed);

    // Swap Chain Beschreibung
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
    swapChainDesc.Windowed = windowed;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Vollbildwechsel zulassen

    // Swap Chain erstellen
    hr = pDXGIFactory->CreateSwapChain(m_pd3dDevice, &swapChainDesc, &m_pSwapChain);
    if (SUCCEEDED(hr))
    {
        Memory::SafeRelease(pDXGIFactory);
    }
    else 
        FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr));

    return hr;
}

HRESULT CDevice::CreateRenderTarget(unsigned int width, unsigned int height)
{
    HRESULT hr = S_OK;

    // Get back-buffer from the Swap Chain
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_pBackBuffer);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        return hr;
    }
    else if (m_pBackBuffer)
    {
        hr = m_pd3dDevice->CreateRenderTargetView(m_pBackBuffer, NULL, &m_pRenderTargetView);
        if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
        {
            Memory::SafeRelease(m_pBackBuffer);
            return hr;
        }
        Memory::SafeRelease(m_pBackBuffer);
    }

    // Return successfully created Render Target
    return hr;
}

HRESULT CDevice::Flip(int syncInterval)
{
    // SyncInterval: This parameter determines how the monitor's refresh rate (vertical synchronization) is used. 
    // A value of 0 means that the VSYNC reset is not considered, and the image is displayed immediately when available. 
    // A value greater than 0 means that the VSYNC reset occurs every SyncInterval - frames. For example, a value of 1 
    // means that the VSYNC reset occurs once every two frames, which corresponds to a refresh rate of 30 Hz on a 60 Hz monitor.
    // 
    // Present the backbuffer to the screen
    return m_pSwapChain->Present(syncInterval, 0);
}

void CDevice::SetVertexShader(ID3D11VertexShader* vs)
{
    m_pContext->VSSetShader(vs, nullptr, 0);
}

void CDevice::SetPixelShader(ID3D11PixelShader* ps)
{
    m_pContext->PSSetShader(ps, nullptr, 0);
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

HRESULT CDevice::CreateDepthBuffer(unsigned int width, unsigned int height)
{
    HRESULT hr = S_OK;

    hr = CreateDepthTexture(width, height);
    if (FAILED(hr))
        return hr;

    hr = CreateDepthStencilView();
    if (FAILED(hr))
        return hr;

    hr = CreateRasterizerState();
    if (FAILED(hr))
        return hr;

    hr = CreateDepthStencilState();
    if (FAILED(hr))
        return hr;

    return hr;
}

HRESULT CDevice::CreateShadowBuffer(unsigned int width, unsigned int height)
{
    HRESULT hr = S_OK;

    // Tiefebuffer ertellen
    hr = CreateShadowMapTexture(width, height);
    if (FAILED(hr))
        return hr;
    
    // Erstellen Ressourcenansichten
    hr = CreateResourceViews();
    if (FAILED(hr))
        return hr;

    hr = CreateComparisonStatus();
    if (FAILED(hr))
        return hr;

    hr = CreateRenderStats();
    if (FAILED(hr))
        return hr;

    return hr;
}

HRESULT CDevice::CreateDepthTexture(unsigned int width, unsigned int height)
{
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

    return GetDevice()->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
}

HRESULT CDevice::CreateDepthStencilView()
{
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    return GetDevice()->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
}

HRESULT CDevice::CreateRasterizerState()
{
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthClipEnable = TRUE;

    return GetDevice()->CreateRasterizerState(&rasterizerDesc, &m_pRasterizerState);
}

HRESULT CDevice::CreateDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    return GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
}

HRESULT CDevice::CreateShadowMapTexture(UINT width, UINT height)
{
    HRESULT hr = S_OK;

    // Schattenmap-Textur erstellen
    D3D11_TEXTURE2D_DESC shadowMapDesc;
    ZeroMemory(&shadowMapDesc, sizeof(shadowMapDesc));
    shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; // Anpassen des Formats je nach Anforderungen
    shadowMapDesc.MipLevels = 1;
    shadowMapDesc.ArraySize = 1;
    shadowMapDesc.SampleDesc.Count = 1;
    shadowMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; // Binden als Tiefen-Stencil-Ziel und Shader-Ressource
    shadowMapDesc.Width = width;
    shadowMapDesc.Height = height;
    //shadowMapDesc.SampleDesc.Quality = 0;
    //shadowMapDesc.Usage = D3D11_USAGE_DEFAULT;
    //shadowMapDesc.CPUAccessFlags = 0;
    //shadowMapDesc.MiscFlags = 0;

    hr = GetDevice()->CreateTexture2D(&shadowMapDesc, NULL, &m_pShadowMap);
    if (FAILED(hr))
        return hr;

    return hr;
}

HRESULT CDevice::CreateResourceViews()
{
    HRESULT hr = S_OK;

    // Depth-Stencil-View für die Schattenmap erstellen
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // Anpassen des Formats je nach Anforderungen
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    hr = GetDevice()->CreateDepthStencilView(m_pShadowMap, &dsvDesc, &m_pShadowMapDepthView);
    if (FAILED(hr))
        return hr;

    // Shader-Ressourcenansicht für die Schattenmap erstellen
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // Anpassen des Formats je nach Anforderungen
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hr = GetDevice()->CreateShaderResourceView(m_pShadowMap, &srvDesc, &m_pShadowMapSRView);
    if (FAILED(hr))
        return hr;

    return hr;
}

HRESULT CDevice::CreateComparisonStatus()
{
    HRESULT hr = S_OK;

    D3D11_SAMPLER_DESC comparisonSamplerDesc;
    ZeroMemory(&comparisonSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
    comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    comparisonSamplerDesc.BorderColor[0] = 1.0f;
    comparisonSamplerDesc.BorderColor[1] = 1.0f;
    comparisonSamplerDesc.BorderColor[2] = 1.0f;
    comparisonSamplerDesc.BorderColor[3] = 1.0f;
    comparisonSamplerDesc.MinLOD = 0.f;
    comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    comparisonSamplerDesc.MipLODBias = 0.f;
    comparisonSamplerDesc.MaxAnisotropy = 0;
    comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;

    // Point filtered shadows can be faster, and may be a good choice when
    // rendering on hardware with lower feature levels. This sample has a
    // UI option to enable/disable filtering so you can see the difference
    // in quality and speed.

    hr = GetDevice()->CreateSamplerState(&comparisonSamplerDesc, &m_pComparisonSampler_point);
    if (FAILED(hr))
        return hr;
    
    return hr;
}

HRESULT CDevice::CreateRenderStats()
{
    HRESULT hr = S_OK;

    D3D11_RASTERIZER_DESC shadowRenderStateDesc;
    ZeroMemory(&shadowRenderStateDesc, sizeof(D3D11_RASTERIZER_DESC));
    shadowRenderStateDesc.CullMode = D3D11_CULL_FRONT;
    shadowRenderStateDesc.FillMode = D3D11_FILL_SOLID;
    shadowRenderStateDesc.DepthClipEnable = true;

    hr = GetDevice()->CreateRasterizerState(&shadowRenderStateDesc, &m_pShadowRenderState);

    return hr;
}

