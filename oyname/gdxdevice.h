#pragma once

namespace gdx
{
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
        DEVICEMANAGER() {};
        ~DEVICEMANAGER() { CleanupResources(); };

		void CleanupResources()
		{
            devices.clear();
        }

        bool IsFormatSupported(unsigned int adapterIndex, GXFORMAT format)
        {
            return (devices[adapterIndex].supportedFormat & format) == format;
        }

        // Getter-Funktionen f�r private Variablen 
        std::vector<GXDEVICE>& GetDevices()  { return devices; }
        
        GXFORMAT GetSupportedFormats(unsigned int adapterIndex)
        {
            return devices[adapterIndex].supportedFormat;
        }

        int GetDirectXVersion(unsigned int adapterIndex)
        {
            return devices[adapterIndex].directxVersion;
        }

        size_t GetNumAdapters() const
        {
            return devices.size();
        }

	};

	class CDevice
	{
	private:
		bool m_bInitialized;

        ID3D11Device*               m_pd3dDevice;
        ID3D11DeviceContext*        m_pContext;

        IDXGISwapChain*             m_pSwapChain;
        ID3D11Texture2D*            m_pBackBuffer;

        ID3D11Texture2D*            m_depthStencilBuffer;
        ID3D11DepthStencilState*    m_depthStencilState;
        ID3D11DepthStencilView*     m_depthStencilView;
        ID3D11RenderTargetView*     m_pRenderTargetView;
        ID3D11RasterizerState*      m_pRasterizerState;

        ID3D11Texture2D*            m_pShadowMap;
        ID3D11ShaderResourceView*   m_pShadowMapSRView;
        ID3D11DepthStencilView*     m_pShadowMapDepthView;
        ID3D11RenderTargetView*     m_pShadowTargetView;
        ID3D11RasterizerState*      m_pShadowRenderState;
        ID3D11SamplerState*         m_pComparisonSampler_point;

        HRESULT GetSystemInfo();

        friend class CGIDX;

        void ResizeWindow(HWND hwnd, unsigned int x, unsigned int y, bool windowed);

        HRESULT CreateDepthTexture(unsigned int width, unsigned int height);
        HRESULT CreateDepthStencilView();
        HRESULT CreateRasterizerState();
        HRESULT CreateDepthStencilState();

        HRESULT CreateResourceViews();
        HRESULT CreateComparisonStatus();
        HRESULT CreateRenderStats();
        HRESULT CreateShadowMapTexture(UINT width, UINT height);
    public:
		DEVICEMANAGER deviceManager;

	public:
		CDevice();
		~CDevice();

        HRESULT Init();
		void Release(void);

        HRESULT InitializeDirectX(IDXGIAdapter* adapter, D3D_FEATURE_LEVEL* featureLevels);
        HRESULT CreateSwapChain(IDXGIFactory* pDXGIFactory,
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

        void SetVertexShader(ID3D11VertexShader* vertexshader);
        void SetPixelShader(ID3D11PixelShader* pixelshader);

        HRESULT Flip(int syncInterval);

        ID3D11Device* GetDevice() const {
            return m_pd3dDevice;}

        ID3D11DeviceContext* GetDeviceContext() const {
            return m_pContext;}

        IDXGISwapChain* GetSwapChain(){
            return m_pSwapChain;}

        ID3D11RenderTargetView* GetTargetView() {
            return m_pRenderTargetView;
        }

        ID3D11Texture2D* GetBackBuffer(){
            m_pBackBuffer;
        }
	};
}