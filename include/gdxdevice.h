#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <vector>
#include "gdxutil.h"  // ← WICHTIG: War vorher nicht included!


struct GXDEVICE
{
	D3D_FEATURE_LEVEL featureLevel;
	unsigned int directxVersion;
	GXFORMAT supportedFormat;
};

struct DEVICEMANAGER
{
private:
	std::vector<GXDEVICE> devices;

public:
	DEVICEMANAGER() {}
	~DEVICEMANAGER() { CleanupResources(); }

	void CleanupResources()
	{
		devices.clear();
	}

	bool IsFormatSupported(unsigned int adapterIndex, GXFORMAT format)
	{
		if (adapterIndex >= devices.size())
			return false;

		return (devices[adapterIndex].supportedFormat & format) == format;
	}

	std::vector<GXDEVICE>& GetDevices()
	{
		return devices;
	}

	GXFORMAT GetSupportedFormats(unsigned int adapterIndex) const
	{
		if (adapterIndex >= devices.size())
			return GXFORMAT(0);

		return devices[adapterIndex].supportedFormat;
	}

	int GetFeatureLevel(unsigned int adapterIndex) const
	{
		if (adapterIndex >= devices.size())
			return 0;

		return devices[adapterIndex].directxVersion;
	}

	size_t GetNumAdapters() const
	{
		return devices.size();
	}
};

class GDXDevice
{
private:
	// Initialization state
	bool m_bInitialized;

	// Device and context
	ID3D11Device* m_pd3dDevice;
	ID3D11DeviceContext* m_pContext;

	// Swap chain and render target
	IDXGISwapChain* m_pSwapChain;
	ID3D11Texture2D* m_pBackBuffer;

	// Depth buffer
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11RasterizerState* m_pRasterizerState;

	// Shadow mapping
	ID3D11Texture2D* m_pShadowMap;
	ID3D11ShaderResourceView* m_pShadowMapSRView;
	ID3D11DepthStencilView* m_pShadowMapDepthView;
	ID3D11RenderTargetView* m_pShadowTargetView;
	ID3D11RasterizerState* m_pShadowRenderState;
	ID3D11SamplerState* m_pComparisonSampler_point;
	ID3D11Buffer* m_shadowMatrixBuffer;

	// Private initialization methods
	HRESULT EnumerateSystemDevices();

	// Depth buffer creation
	HRESULT CreateDepthTexture(unsigned int width, unsigned int height);
	HRESULT CreateDepthStencilView();
	HRESULT CreateRasterizerState();
	HRESULT CreateDepthStencilState();

	// Shadow map creation
	HRESULT CreateShadowMapTexture(UINT width, UINT height);
	HRESULT CreateResourceViews();
	HRESULT CreateComparisonStatus();
	HRESULT CreateRenderStats();
	HRESULT CreateShadowMatrixBuffer();

	// Window management
	void ResizeWindow(HWND hwnd, unsigned int x, unsigned int y, bool windowed);

	friend class GDXEngine;

public:
	DEVICEMANAGER deviceManager;  // ← Bleibt hier

public:
	GDXDevice();
	~GDXDevice();

	// Initialization
	HRESULT Init();
	void Release();

	// Device initialization
	HRESULT InitializeDirectX(IDXGIAdapter* adapter, D3D_FEATURE_LEVEL* featureLevel);

	// Swap chain and render targets
	HRESULT CreateSwapChain(
		IDXGIFactory* pDXGIFactory,
		HWND hWnd,
		unsigned int width,
		unsigned int height,
		DXGI_FORMAT format,
		unsigned int numerator,
		unsigned int denominator,
		bool windowed);

	HRESULT CreateRenderTarget(unsigned int width, unsigned int height);
	HRESULT CreateDepthBuffer(unsigned int width, unsigned int height);
	HRESULT CreateShadowBuffer(unsigned int width, unsigned int height);

	// Shader setting
	void SetVertexShader(ID3D11VertexShader* vertexShader);
	void SetPixelShader(ID3D11PixelShader* pixelShader);

	// Presentation
	HRESULT Flip(int syncInterval);

	// Getters
	ID3D11Device* GetDevice() const
	{
		return m_pd3dDevice;
	}

	ID3D11RasterizerState* GetRasterizerState() const
	{
		return m_pRasterizerState;
	}

	ID3D11DeviceContext* GetDeviceContext() const
	{
		return m_pContext;
	}

	IDXGISwapChain* GetSwapChain() const
	{
		return m_pSwapChain;
	}

	ID3D11RenderTargetView* GetRenderTargetView() const
	{
		return m_pRenderTargetView;
	}

	// Rückwärtskompatibilität
	ID3D11RenderTargetView* GetTargetView() const
	{
		return m_pRenderTargetView;
	}

	ID3D11Texture2D* GetBackBuffer() const
	{
		return m_pBackBuffer;
	}

	ID3D11DepthStencilView* GetDepthStencilView() const
	{
		return m_depthStencilView;
	}

	ID3D11DepthStencilView* GetShadowMapDepthView() const
	{
		return m_pShadowMapDepthView;
	}

	ID3D11ShaderResourceView* GetShadowMapSRV() const
	{
		return m_pShadowMapSRView;
	}

	ID3D11SamplerState* GetComparisonSampler() const
	{
		return m_pComparisonSampler_point;
	}

	ID3D11RasterizerState* GetShadowRasterState() const
	{
		return m_pShadowRenderState;
	}

	ID3D11Buffer* GetShadowMatrixBuffer() const
	{
		return m_shadowMatrixBuffer;
	}

	// Shadow map dimensions (queried from the texture).
	// Returns (0,0) if the shadow map is not created.
	void GetShadowMapSize(UINT& outWidth, UINT& outHeight) const
	{
		outWidth = 0;
		outHeight = 0;
		if (!m_pShadowMap) return;
		D3D11_TEXTURE2D_DESC desc{};
		m_pShadowMap->GetDesc(&desc);
		outWidth = desc.Width;
		outHeight = desc.Height;
	}

	bool IsInitialized() const
	{
		return m_bInitialized;
	}
};
