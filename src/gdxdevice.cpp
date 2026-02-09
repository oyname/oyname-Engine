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
        Memory::SafeRelease(m_pRasterizerState);
        Memory::SafeRelease(m_pShadowRenderState);
        Memory::SafeRelease(m_pComparisonSampler_point);
    }

    m_bInitialized = false;
}

HRESULT CDevice::Init()
{
    return EnumerateSystemDevices();
}

HRESULT CDevice::EnumerateSystemDevices()
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

    IDXGIFactory* factory = nullptr;
    IDXGIAdapter* adapter = nullptr;

    hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory));
    if (FAILED(hr))
    {
        Debug::GetErrorMessage(__FILE__, __LINE__, hr);
        return hr;
    }

    // Enumerate adapters and test device creation
    for (UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        if (!adapter)
            continue;

        // Skip Windows renderer
        if (GXUTIL::isWindowsRenderer(adapter))
        {
            Memory::SafeRelease(adapter);
            continue;
        }

        // Test device creation
        ID3D11Device* testDevice = nullptr;
        D3D_FEATURE_LEVEL achievedFeatureLevel;

        hr = D3D11CreateDevice(
            adapter,
            D3D_DRIVER_TYPE_UNKNOWN,
            nullptr,
            0,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            &testDevice,
            &achievedFeatureLevel,
            nullptr);

        if (SUCCEEDED(hr) && testDevice)
        {
            GXDEVICE gxDevice = {};
            gxDevice.featureLevel = achievedFeatureLevel;
            gxDevice.supportedFormat = GXUTIL::GetSupportedFormats(achievedFeatureLevel);
            gxDevice.directxVersion = GXUTIL::GetDirectXVersion(gxDevice.featureLevel);

            deviceManager.GetDevices().push_back(gxDevice);

            // Release test device immediately
            Memory::SafeRelease(testDevice);
        }
        else
        {
            Debug::GetErrorMessage(__FILE__, __LINE__, hr);
        }

        Memory::SafeRelease(adapter);
    }

    Memory::SafeRelease(factory);

    m_bInitialized = SUCCEEDED(hr);
    return hr;
}

HRESULT CDevice::InitializeDirectX(IDXGIAdapter* adapter, D3D_FEATURE_LEVEL* featureLevel)
{
    if (!adapter || !featureLevel)
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    // Create the Direct3D device with single feature level
    hr = D3D11CreateDevice(
        adapter,
        D3D_DRIVER_TYPE_UNKNOWN,
        nullptr,
        0,
        featureLevel,   // ← Pass pointer to feature level array
        1,              // ← Count is 1
        D3D11_SDK_VERSION,
        &m_pd3dDevice,
        nullptr,
        &m_pContext);

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
    if (!pDXGIFactory || !hWnd || !m_pd3dDevice)
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    // Adjust window size
    ResizeWindow(hWnd, width, height, windowed);

    // Swap Chain Description
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = (format != DXGI_FORMAT_UNKNOWN) ? format : DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = (numerator != 0) ? numerator : 1;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = (denominator != 0) ? denominator : 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = windowed;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // Create Swap Chain
    hr = pDXGIFactory->CreateSwapChain(m_pd3dDevice, &swapChainDesc, &m_pSwapChain);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        return hr;
    }

    // Don't release factory here - caller manages it

    return hr;
}

HRESULT CDevice::CreateRenderTarget(unsigned int width, unsigned int height)
{
    // Width and height are determined by the swap chain, 
    // these parameters are kept for backward compatibility but not used

    if (!m_pSwapChain || !m_pd3dDevice)
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    // Get back-buffer from Swap Chain
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_pBackBuffer);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        return hr;
    }

    // Create render target view
    hr = m_pd3dDevice->CreateRenderTargetView(m_pBackBuffer, NULL, &m_pRenderTargetView);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        Memory::SafeRelease(m_pBackBuffer);
        return hr;
    }

    Memory::SafeRelease(m_pBackBuffer);

    return hr;
}

HRESULT CDevice::Flip(int syncInterval)
{
    if (!m_pSwapChain)
        return E_INVALIDARG;

    return m_pSwapChain->Present(syncInterval, 0);
}

void CDevice::SetVertexShader(ID3D11VertexShader* vs)
{
    if (m_pContext)
        m_pContext->VSSetShader(vs, nullptr, 0);
}

void CDevice::SetPixelShader(ID3D11PixelShader* ps)
{
    if (m_pContext)
        m_pContext->PSSetShader(ps, nullptr, 0);
}

void CDevice::ResizeWindow(HWND hwnd, unsigned int x, unsigned int y, bool windowed)
{
    if (!hwnd || x == 0 || y == 0)
        return;

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
    if (width == 0 || height == 0 || !m_pd3dDevice)
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    // Create depth texture
    hr = CreateDepthTexture(width, height);
    if (FAILED(hr))
        return hr;

    // Create depth stencil view
    hr = CreateDepthStencilView();
    if (FAILED(hr))
        goto cleanup;

    // Create rasterizer state
    hr = CreateRasterizerState();
    if (FAILED(hr))
        goto cleanup;

    // Create depth stencil state
    hr = CreateDepthStencilState();
    if (FAILED(hr))
        goto cleanup;

    return hr;

cleanup:
    Memory::SafeRelease(m_depthStencilBuffer);
    Memory::SafeRelease(m_depthStencilView);
    Memory::SafeRelease(m_pRasterizerState);
    return hr;
}

HRESULT CDevice::CreateShadowBuffer(unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0 || !m_pd3dDevice)
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    // Create shadow map texture
    hr = CreateShadowMapTexture(width, height);
    if (FAILED(hr))
        return hr;

    // Create resource views
    hr = CreateResourceViews();
    if (FAILED(hr))
        goto cleanup;

    // Create comparison sampler
    hr = CreateComparisonStatus();
    if (FAILED(hr))
        goto cleanup;

    // Create render state for shadow pass
    hr = CreateRenderStats();
    if (FAILED(hr))
        goto cleanup;

    return hr;

cleanup:
    Memory::SafeRelease(m_pShadowMap);
    Memory::SafeRelease(m_pShadowMapDepthView);
    Memory::SafeRelease(m_pShadowMapSRView);
    Memory::SafeRelease(m_pComparisonSampler_point);
    Memory::SafeRelease(m_pShadowRenderState);
    return hr;
}

HRESULT CDevice::CreateDepthTexture(unsigned int width, unsigned int height)
{
    if (!m_pd3dDevice)
        return E_INVALIDARG;

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

    HRESULT hr = m_pd3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
        return hr;

    return hr;
}

HRESULT CDevice::CreateDepthStencilView()
{
    if (!m_pd3dDevice || !m_depthStencilBuffer)
        return E_INVALIDARG;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    HRESULT hr = m_pd3dDevice->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
        return hr;

    return hr;
}

HRESULT CDevice::CreateRasterizerState()
{
    if (!m_pd3dDevice)
        return E_INVALIDARG;

    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthClipEnable = TRUE;

    HRESULT hr = m_pd3dDevice->CreateRasterizerState(&rasterizerDesc, &m_pRasterizerState);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
        return hr;

    return hr;
}

HRESULT CDevice::CreateDepthStencilState()
{
    if (!m_pd3dDevice)
        return E_INVALIDARG;

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depthStencilDesc.StencilEnable = false;

    HRESULT hr = m_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
        return hr;

    return hr;
}

HRESULT CDevice::CreateShadowMapTexture(UINT width, UINT height)
{
    if (!m_pd3dDevice)
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC shadowMapDesc;
    ZeroMemory(&shadowMapDesc, sizeof(shadowMapDesc));
    shadowMapDesc.Width = width;
    shadowMapDesc.Height = height;
    shadowMapDesc.MipLevels = 1;
    shadowMapDesc.ArraySize = 1;
    shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;  // Match with DSV/SRV formats
    shadowMapDesc.SampleDesc.Count = 1;
    shadowMapDesc.SampleDesc.Quality = 0;
    shadowMapDesc.Usage = D3D11_USAGE_DEFAULT;
    shadowMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    shadowMapDesc.CPUAccessFlags = 0;
    shadowMapDesc.MiscFlags = 0;

    hr = m_pd3dDevice->CreateTexture2D(&shadowMapDesc, NULL, &m_pShadowMap);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
        return hr;

    return hr;
}

HRESULT CDevice::CreateResourceViews()
{
    if (!m_pd3dDevice || !m_pShadowMap)
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    // Create Depth-Stencil-View for shadow map
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;  // ← Match with texture format
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    hr = m_pd3dDevice->CreateDepthStencilView(m_pShadowMap, &dsvDesc, &m_pShadowMapDepthView);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
        return hr;

    // Create Shader Resource View for shadow map
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;  // ← Match with texture format
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    hr = m_pd3dDevice->CreateShaderResourceView(m_pShadowMap, &srvDesc, &m_pShadowMapSRView);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        Memory::SafeRelease(m_pShadowMapDepthView);
        return hr;
    }

    return hr;
}

HRESULT CDevice::CreateComparisonStatus()
{
    if (!m_pd3dDevice)
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    D3D11_SAMPLER_DESC comparisonSamplerDesc;
    ZeroMemory(&comparisonSamplerDesc, sizeof(comparisonSamplerDesc));
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

    hr = m_pd3dDevice->CreateSamplerState(&comparisonSamplerDesc, &m_pComparisonSampler_point);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
        return hr;

    return hr;
}

HRESULT CDevice::CreateRenderStats()
{
    if (!m_pd3dDevice)
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    D3D11_RASTERIZER_DESC shadowRenderStateDesc;
    ZeroMemory(&shadowRenderStateDesc, sizeof(shadowRenderStateDesc));
    shadowRenderStateDesc.CullMode = D3D11_CULL_FRONT;
    shadowRenderStateDesc.FillMode = D3D11_FILL_SOLID;
    shadowRenderStateDesc.DepthClipEnable = true;
    shadowRenderStateDesc.FrontCounterClockwise = FALSE;

    hr = m_pd3dDevice->CreateRasterizerState(&shadowRenderStateDesc, &m_pShadowRenderState);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
        return hr;

    return hr;
}
