#include "gdxengine.h"
#include <fstream>

namespace gdx
{
	// Static-Variablen
	bool CGIDX::running = true;
	double CGIDX::deltaTime = 0.0;
	double CGIDX::accumulator = 0.0;
	Timer::TimeMode timeMode = Timer::TimeMode::FIXED_TIMESTEP;  // Standard
	std::chrono::high_resolution_clock::time_point CGIDX::lastFrameTime = std::chrono::high_resolution_clock::now();
	CGIDX* CGIDX::s_instance = nullptr;

	// Hilfsfunktion: Konvertiere relativen Pfad zu absolutem Pfad
	std::wstring ResolveAbsolutePath(const std::wstring& relativePath) {
		wchar_t absolutePath[MAX_PATH];

		// Windows API: Löse relative Pfade auf
		DWORD result = GetFullPathNameW(relativePath.c_str(), MAX_PATH, absolutePath, nullptr);

		if (result == 0 || result >= MAX_PATH) {
			// Fehler beim Auflösen - gib Original zurück
			Debug::Log("WARNING: Could not resolve path: ", relativePath.c_str());
			return relativePath;
		}

		return std::wstring(absolutePath);
	}

	static std::wstring GetExeDir()
	{
		wchar_t path[MAX_PATH]{};
		GetModuleFileNameW(nullptr, path, MAX_PATH);
		std::wstring p(path);
		auto pos = p.find_last_of(L"\\/");
		return (pos == std::wstring::npos) ? L"" : p.substr(0, pos + 1);
	}

	//
	CGIDX::CGIDX(HWND hwnd, HINSTANCE hinst, unsigned int bpp, unsigned int screenX, unsigned int screenY, int* result) :
		m_objectManager(),
		m_lightManager(),
		m_renderManager(m_objectManager, m_lightManager)
	{
		m_colorDepth = bpp;
		m_screenWidth = screenX;
		m_screenHeight = screenY;
		m_hwnd = hwnd;

		m_globalAmbient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);  // Standard Ambient
		s_instance = this;  // Singleton setzen
		m_device.Init();

		m_interface.Init(bpp);

		// Primary adapter = 0
		this->SetAdapter(0);

		std::wstring exeDir = GetExeDir();

		vs = ResolveAbsolutePath(exeDir + L"..\\..\\shaders\\VertexShader.hlsl");
		ps = ResolveAbsolutePath(exeDir + L"..\\..\\shaders\\PixelShader.hlsl");

		// Prüfe ob Dateien existieren
		std::wifstream vsFile(vs);
		std::wifstream psFile(ps);

		if (!vsFile.good()) {
			Debug::Log("Vertex Shader NOT FOUND!");
		}

		if (!psFile.good()) {
			Debug::Log("Pixel Shader NOT FOUND!");
		}

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
		if (FAILED(hr))
		{
			Debug::LogHr(__FILE__, __LINE__, hr);
			return hr;
		}

		// Create device
		hr = m_device.InitializeDirectX(adapter, &featureLevel);
		if (FAILED(hr))
		{
			Debug::LogHr(__FILE__, __LINE__, hr);
			return hr;
		}

		Memory::SafeRelease(adapter);

		// Frequency
		unsigned int numerator = m_interface.interfaceManager.GetNumerator(this->GetAdapterIndex(), this->GetOutputIndex(), width, height);
		unsigned int denominator = m_interface.interfaceManager.GetDenominator(this->GetAdapterIndex(), this->GetOutputIndex(), width, height);

		// Create the SwapChain
		hr = m_device.CreateSwapChain(m_interface.GetFactory(), GetHWND(), width, height, m_interface.interfaceManager.GetDXGI_Format(), numerator, denominator, windowed);
		if (FAILED(hr))
		{
			Debug::LogHr(__FILE__, __LINE__, hr);
			return hr;
		}

		// Create Backbuffer
		hr = m_device.CreateRenderTarget(width, height);
		if (FAILED(hr))
		{
			Debug::LogHr(__FILE__, __LINE__, hr);
			return hr;
		}

		// Initialization of the Depth-Stencil buffer and views
		hr = m_device.CreateDepthBuffer(width, height);
		if (FAILED(hr))
		{
			Debug::LogHr(__FILE__, __LINE__, hr);
			return hr;
		}

		hr = m_device.CreateShadowBuffer(2048, 2048);
		if (FAILED(hr))
		{
			Debug::LogHr(__FILE__, __LINE__, hr);
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

		// ...and load - VERWENDE DIE DYNAMISCHEN PFADE!
		hr = GetSM().CreateShader(GetSM().GetShader(), vs.c_str(), "main", ps.c_str(), "main");
		if (FAILED(hr))
		{
			Debug::LogHr(__FILE__, __LINE__, hr);
			return hr;
		}

		// Create standard material and add to standard shader
		GetOM().addMaterialToShader(GetSM().GetShader(), GetOM().createMaterial());

		// ==================== CREATE MATERIAL CONSTANT BUFFER ====================
		LPMATERIAL standardMaterial = GetSM().GetShader()->materials.front();

		hr = GetBM().CreateBuffer(
			&standardMaterial->properties,
			sizeof(Material::MaterialData),
			1,
			D3D11_BIND_CONSTANT_BUFFER,
			&standardMaterial->materialBuffer
		);

		if (FAILED(hr))
		{
			Debug::LogHr(__FILE__, __LINE__, hr);
			return hr;
		}

		// Create layout for the vertices
		hr = GetILM().CreateInputLayoutVertex(&GetSM().GetShader()->inputlayoutVertex,	// Store the layout
			GetSM().GetShader(),														// The shader object
			GetSM().GetShader()->flagsVertex,											// Store the flag
			D3DVERTEX_POSITION | D3DVERTEX_COLOR | D3DVERTEX_NORMAL | D3DVERTEX_TEX1);

		if (FAILED(hr))
		{
			Debug::LogHr(__FILE__, __LINE__, hr);
			return hr;
		}

		m_screenHeight = height;
		m_screenWidth = width;

		return hr;
	}

	HRESULT CGIDX::RenderWorld()
	{
		HRESULT hr = S_OK;

		Timer::GetInstance()->Update();

		auto* pContext = this->m_device.GetDeviceContext();
		if (!pContext)
		{
			Debug::Log("RenderWorld: Device Context is null.");
			return E_FAIL;
		}

		auto* pCamera = m_cameraManager.GetCurrentCam();
		if (!pCamera)
		{
			Debug::Log("RenderWorld: No valid camera found.");
			return E_FAIL;
		}

		// Clear (kann im Engine bleiben; Owner der Pipeline-States ist trotzdem RenderManager)
		ID3D11DepthStencilView* dsv = this->m_device.GetDepthStencilView();
		if (!dsv)
			return E_POINTER;

		pContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Wichtig: RenderManager bekommt deterministisch die Camera
		m_renderManager.SetCamera(pCamera);

		// RenderManager setzt OM/RS/Viewport pro Pass selbst
		m_renderManager.RenderScene();

		return hr;
	}

	void CGIDX::UpdateWorld()
	{
		auto* cam = m_cameraManager.GetCurrentCam();

		if (cam == nullptr) {
			Debug::Log("ERROR: UpdateWorld - No camera set");
			return;
		}

		DirectX::XMVECTOR position = cam->transform.getPosition();
		DirectX::XMVECTOR forward = DirectX::XMVector3Normalize(cam->transform.getLookAt());
		DirectX::XMVECTOR up = DirectX::XMVector3Normalize(cam->transform.getUp());

		// Funktioniert - cam ist Camera*
		cam->UpdateCamera(position, forward, up);

		// Light
		m_lightManager.Update(&m_device);
	}

	HRESULT CGIDX::Cls(float r, float g, float b, float a)
	{
		HRESULT hr = S_OK;

		//Clear our backbuffer to the updated color
		float color[4] = { r, g, b, a };

		m_device.GetDeviceContext()->ClearRenderTargetView(m_device.GetTargetView(), color);

		hr = m_device.GetDevice()->GetDeviceRemovedReason(); // Check for device error
		if (FAILED(hr))
		{
			Debug::LogHr(__FILE__, __LINE__, hr);
			return hr;
		}

		return hr;
	}

	void CGIDX::UpdateFrameTime()
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		double rawDeltaTime = std::chrono::duration<double>(currentTime - lastFrameTime).count();
		lastFrameTime = currentTime;

		if (rawDeltaTime > 0.1) {
			rawDeltaTime = 0.1;
		}

		// === MODUS-ABHÄNGIG ===
		if (timeMode == Timer::TimeMode::FIXED_TIMESTEP) {
			// Feste Timestep (60 FPS)
			accumulator += rawDeltaTime;

			if (accumulator >= FIXED_TIMESTEP) {
				deltaTime = FIXED_TIMESTEP;
				accumulator -= FIXED_TIMESTEP;
			}
			else {
				deltaTime = 0.0;  // Kein Update diese Frame
			}

			// Frame-Rate-Limiting
			double targetFrameTime = 1.0 / TARGET_FPS;
			if (rawDeltaTime < targetFrameTime) {
				std::this_thread::sleep_for(
					std::chrono::duration<double>(targetFrameTime - rawDeltaTime)
				);
			}
		}
		else if (timeMode == Timer::TimeMode::VSYNC_ONLY) {
			// Variable Delta-Time (nur VSync)
			deltaTime = rawDeltaTime;
			// Kein Sleep, kein Fixed Timestep
		}
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

	void CGIDX::SetCamera(LPENTITY entity)
	{
		if (entity == nullptr) {
			Debug::Log("ERROR: SetCamera - entity is nullptr");
			return;
		}

		// Type-Check mit dynamic_cast
		Camera* camera = dynamic_cast<Camera*>(entity);
		if (camera == nullptr) {
			Debug::Log("ERROR: SetCamera - Entity is not a Camera!");
			return;
		}

		GetCam().SetCamera(camera);         // ← Camera* an CameraManager
		m_renderManager.SetCamera(entity);  // ← Entity* an RenderManager (OK)
	}

	void CGIDX::SetDirectionalLight(LPENTITY entity)
	{
		if (entity == nullptr) {
			Debug::Log("ERROR: SetDirectionalLight - entity is nullptr");
			return;
		}

		// Type-Check mit dynamic_cast
		Light* light = dynamic_cast<Light*>(entity);
		if (light == nullptr) {
			Debug::Log("ERROR: SetDirectionalLight - Entity is not a Light!");
			return;
		}

		m_renderManager.SetDirectionalLight(entity);
		Debug::Log("gdxengine.cpp: SetDirectionalLight - Directional Light set for Shadow Mapping");
	}
}