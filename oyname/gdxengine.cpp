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

		// Primäradapter = 0
		this->SetAdapter(0);

		m_bInitialized = true;
	}

	CGIDX::~CGIDX()
	{
		if(m_bInitialized)
			this->Cleanup();
	}

	void CGIDX::Cleanup()
	{
		if (m_bInitialized)
		{
			// Aufräumarbeiten
		}

		m_bInitialized = false;
	}

	HRESULT CGIDX::Graphic(unsigned int width, unsigned int height, bool windowed)
	{
		HRESULT hr = S_OK;

		// Index aktueller Adapter (0 = Primär)
		int index = GetAdapterIndex();

		// Adapter
		IDXGIAdapter* adapter = nullptr;

		// Aktueller Adapter mit der unterstützten DiretcX Version
		D3D_FEATURE_LEVEL featureLevel;
		featureLevel = Common::GetFeatureLevelFromDirectXVersion(m_device.deviceManager.GetDirectXVersion(index));

		// Adapter erstellen mit aktuellem Index
		hr = m_interface.GetFactory()->EnumAdapters(index, &adapter);
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		// Device erstellen
		hr = m_device.InitializeDirectX(adapter, &featureLevel);
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		Memory::SafeRelease(adapter);

		// Frequenz
		unsigned int numerator = m_interface.interfaceManager.GetNumerator(this->GetAdapterIndex(), this->GetOutputIndex(), width, height);
		unsigned int denomiator = m_interface.interfaceManager.GetDenominator(this->GetAdapterIndex(), this->GetOutputIndex(), width, height);

		// Erstelle die SwapChain
		hr = m_device.CreateSwapChain(m_interface.GetFactory(), GetHWND(), width, height, m_interface.interfaceManager.GetDXGI_Format(), numerator, denomiator, windowed);
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		// Backbuffer erstellen
		hr = m_device.CreateRenderTarget(width, height);
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		// Setze Rendertarget
		m_device.SetRenderTargets();

		// Initialisierung des Depth-Stencil-Buffers und -Views
		hr = m_device.CreateDeepBuffer(width, height);
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		// Objekt zum organisieren des Renders
		
		// 1. Objekte initialisieren
		
		// Camera 
		m_camera.Init(this);
		m_camera.SetViewport(0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f);
		m_device.CreateView(1, m_camera.GetViewPort());
		
		// Objektmanager
		m_objectManager.Init(&m_device);
		
		// Buffer-Manager initialisieren
		m_bufferManager.Init(m_device.GetDevice());
		
		// Shader-Manager initialisieren
		m_shaderManager.Init(m_device.GetDevice());
		
		// Input-Layout Managers initialisieren
		m_inputLayoutManager.Init(m_device.GetDevice());
		
		// 2. Standardzeug erstellen
		
		// Standardshader erstellen.
		GetSM().SetShader(m_objectManager.createShader());
		// ...und laden
		hr = GetSM().CreateShader(GetSM().GetShader(), L"vertexshader.hlsl", L"pixelshader.hlsl","main");
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		// Layout für die Vertices erstellen
		hr = GetILM().CreateInputLayoutVertex(&GetSM().GetShader()->inputlayoutVertex, // Speichert das Layout
											   GetSM().GetShader(),                    // Das Shader-Objekt
			                                   GetSM().GetShader()->flagsVertex,       // Speichert das Flag
			                                   D3DVERTEX_POSITION | D3DVERTEX_COLOR | D3DVERTEX_NORMAL);
		if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
			return hr;
		}

		m_screenHeight = height;
		m_screenWidth = width;

		return hr;
	}

	void CGIDX::RenderWorld()
	{
		m_camera.Update();
		m_renderManager.RenderLoop();
	}

	void CGIDX::UpdateWorld()
	{
		
	}

	HRESULT CGIDX::Cls(float r, float g, float b, float a)
	{
		HRESULT hr = S_OK;

		//Clear our backbuffer to the updated color
		float color[4] = { r, g, b, a };

		m_device.GetDeviceContext()->ClearRenderTargetView(m_device.GetTargetView(), color);

		hr = m_device.GetDevice()->GetDeviceRemovedReason(); // Überprüfe auf Gerätefehler
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

	Camera& CGIDX::GetCam() {
		return m_camera;
	}

	void CGIDX::SetAdapter(unsigned int index)
	{
		m_adapterIndex = index;
	}

	void CGIDX::SetOutput(unsigned int index)
	{
		m_monitorIndex = index;
	}

	void CGIDX::SetCamera(LPMESH mesh)
	{
		GetCam().SetCamera(mesh);
		m_renderManager.SetCamera(mesh);
	}

	void CGIDX::SetDirectionalLight(LPLIGHT dirLight)
	{
		m_renderManager.SetDirectionalLight(dirLight);
	}

	void CGIDX::SetFormat(DXGI_FORMAT bufferFormat)
	{
		// ....
	}
}