#include "gdxengine.h"

namespace gdx
{
	CGIDX::CGIDX(HWND hwnd, HINSTANCE hinst, unsigned int bpp, unsigned int screenX, unsigned int screenY, int* result) :
		m_objectManager(),
		m_lightManager(),
		m_renderManager(m_objectManager, m_lightManager)
	{
		m_colorDepth = bpp;
		m_screenWidth = screenX;
		m_screenHeight = screenY;
		m_hwnd = hwnd;

		m_device.Init();

		m_interface.Init(bpp);

		// Primary adapter = 0
		this->SetAdapter(0);

		m_bInitialized = true;
	}

	CGIDX::~CGIDX()
	{
		if (m_bInitialized)
			this->Cleanup();
	}

	void CGIDX::Cleanup()
	{
		if (m_bInitialized)
		{
			// Clean-up operations
		}

		m_bInitialized = false;
	}

	HRESULT CGIDX::Graphic(unsigned int width, unsigned int height, bool windowed)
	{
		HRESULT hr = S_OK;

		// Index of current adapter (0 = Primary)
		int index = GetAdapterIndex();

		// Adapter
		IDXGIAdapter* adapter = nullptr;

		// Current adapter with supported DirectX version
		D3D_FEATURE_LEVEL featureLevel;
		featureLevel = GXUTIL::GetFeatureLevelFromDirectXVersion(m_device.deviceManager.GetDirectXVersion(index));

		// Create adapter with current index
		hr = m_interface.GetFactory()->EnumAdapters(index, &adapter);
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		// Create device
		hr = m_device.InitializeDirectX(adapter, &featureLevel);
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		Memory::SafeRelease(adapter);

		// Frequency
		unsigned int numerator = m_interface.interfaceManager.GetNumerator(this->GetAdapterIndex(), this->GetOutputIndex(), width, height);
		unsigned int denominator = m_interface.interfaceManager.GetDenominator(this->GetAdapterIndex(), this->GetOutputIndex(), width, height);

		// Create the SwapChain
		hr = m_device.CreateSwapChain(m_interface.GetFactory(), GetHWND(), width, height, m_interface.interfaceManager.GetDXGI_Format(), numerator, denominator, windowed);
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		// Create Backbuffer
		hr = m_device.CreateRenderTarget(width, height);
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		// Initialization of the Depth-Stencil buffer and views
		hr = m_device.CreateDepthBuffer(width, height);
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		hr = m_device.CreateShadowBuffer(width, height);
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		// 1. Initialize objects
		//
		// Object manager
		m_objectManager.Init(&m_device);

		// Initialize Buffer Manager
		m_bufferManager.Init(m_device.GetDevice(), m_device.GetDeviceContext());

		// Initialize Shader Manager
		m_shaderManager.Init(m_device.GetDevice());

		// Initialize Input-Layout Managers
		m_inputLayoutManager.Init(m_device.GetDevice());

		// Create standard shader.
		GetSM().SetShader(m_objectManager.createShader());
		// ...and load
		hr = GetSM().CreateShader(GetSM().GetShader(), VERTEX_SHADER_FILE, "main", PIXEL_SHADER_FILE, "main");
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		// Create standard brush and add to standard shader
		GetOM().addBrushToShader(GetSM().GetShader(), GetOM().createBrush());

		// Create layout for the vertices
		hr = GetILM().CreateInputLayoutVertex(&GetSM().GetShader()->inputlayoutVertex,	// Store the layout
			 GetSM().GetShader(),														// The shader object
			 GetSM().GetShader()->flagsVertex,											// Store the flag
			 D3DVERTEX_POSITION | D3DVERTEX_COLOR | D3DVERTEX_NORMAL | D3DVERTEX_TEX1);

		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		m_screenHeight = height;
		m_screenWidth = width;

		return hr;
	}

	void CGIDX::RenderWorld()
	{
		this->m_lightManager.Update(&this->m_device);
		
		this->m_device.GetDeviceContext()->ClearDepthStencilView(this->m_device.m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		this->m_device.GetDeviceContext()->OMSetRenderTargets(1, &this->m_device.m_pRenderTargetView, this->m_device.m_depthStencilView);
		this->m_device.GetDeviceContext()->RSSetState(this->m_device.m_pRasterizerState);
		this->m_device.GetDeviceContext()->RSSetViewports(1, &m_cameraManager.GetCurrentCam()->viewport);

		m_renderManager.RenderScene();
	}

	void CGIDX::UpdateWorld()
	{
		XMVECTOR position = m_cameraManager.GetCurrentCam()->transform.getPosition();
		XMVECTOR lookAt = m_cameraManager.GetCurrentCam()->transform.getLookAt();
		XMVECTOR up = m_cameraManager.GetCurrentCam()->transform.getUp();
		m_cameraManager.GetCurrentCam()->UpdateCamera(position, lookAt, up);
	}

	HRESULT CGIDX::Cls(float r, float g, float b, float a)
	{
		HRESULT hr = S_OK;

		//Clear our backbuffer to the updated color
		float color[4] = { r, g, b, a };

		m_device.GetDeviceContext()->ClearRenderTargetView(m_device.GetTargetView(), color);

		hr = m_device.GetDevice()->GetDeviceRemovedReason(); // Check for device error
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
		{
			return hr;
		}

		return hr;
	}

	HWND CGIDX::GetHWND()
	{
		return m_hwnd;
	}

	unsigned int CGIDX::GetAdapterIndex()
	{
		return m_adapterIndex;
	}

	unsigned int CGIDX::GetOutputIndex()
	{
		return m_monitorIndex;
	}

	unsigned int CGIDX::GetWidth()
	{
		return m_screenWidth;
	}

	unsigned int CGIDX::GetHeight()
	{
		return m_screenHeight;
	}

	unsigned int CGIDX::GetColorDepth()
	{
		return m_colorDepth;
	}

	BufferManager& CGIDX::GetBM() {
		return m_bufferManager;
	}

	ObjectManager& CGIDX::GetOM() {
		return m_objectManager;
	}

	ShaderManager& CGIDX::GetSM() {
		return m_shaderManager;
	}

	LightManager& CGIDX::GetLM() {
		return m_lightManager;
	}

	InputLayoutManager& CGIDX::GetILM() {
		return m_inputLayoutManager;
	}

	TextureManager& CGIDX::GetTM() {
		return m_texturManager;
	}

	CameraManager& CGIDX::GetCam() {
		return m_cameraManager;
	}

	void CGIDX::SetAdapter(unsigned int index)
	{
		m_adapterIndex = index;
	}

	void CGIDX::SetOutput(unsigned int index)
	{
		m_monitorIndex = index;
	}

	void CGIDX::SetCamera(LPENTITY mesh)
	{
		GetCam().SetCamera(mesh);
		m_renderManager.SetCamera(mesh);
	}
}