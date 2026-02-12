#pragma once

#include <windows.h>
#include "gdxutil.h"
#include "gdxinterface.h"
#include "gdxdevice.h"
#include "InputLayoutManager.h"
#include "ObjectManager.h"
#include "BufferManager.h"
#include "ShaderManager.h"
#include "RenderManager.h"
#include "LightManager.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include "Transform.h"
#include "Timer.h"
#include <thread>

#define VERTEX_SHADER_FILE L"shaders/VertexShader.hlsl" 
#define PIXEL_SHADER_FILE L"shaders/PixelShader.hlsl"

namespace Windows
{
	// MainLoop is used in gidx.cpp
	void MainLoop(bool running);

	bool MainLoop();

	// Shthe main thread
	int ShutDown();
}

namespace gdx {

	class CGIDX {
	private:
		bool m_bInitialized;
		HWND m_hwnd;				// Window handle
		int m_colorDepth;			// Color depth (e.g., 32-bit colors)
		int m_screenWidth;			// Screen width
		int m_screenHeight;			// Screen height	
		int m_adapterIndex;			// Current adapter index
		int m_monitorIndex;			// Index of the monitor
		static CGIDX* s_instance;	// Singleton-Pointer

		DirectX::XMFLOAT4 m_globalAmbient;  // Globale Ambient-Farbe für die Szene

		std::wstring vs;
		std::wstring ps;

		// Manager classes
		ObjectManager       m_objectManager;
		RenderManager		m_renderManager;
		ShaderManager		m_shaderManager;
		InputLayoutManager	m_inputLayoutManager;
		BufferManager		m_bufferManager;
		LightManager		m_lightManager;
		TextureManager		m_texturManager;
		CameraManager		m_cameraManager;

	public:
		CDevice m_device;		// Device Manager, not to be confused with DirectXDevice
		CInterface m_interface;	// Interface Manager

		CGIDX(HWND hwnd, HINSTANCE hinst, unsigned int bpp, unsigned int screenX, unsigned int screenY, int* result);
		~CGIDX();

		static CGIDX* GetInstance() { return s_instance; }

		HRESULT RenderWorld();
		void Cleanup();
		void UpdateWorld();

		// 
		HRESULT Graphic(unsigned int width, unsigned int height, bool windowed);
		HRESULT Cls(float r, float g, float b, float a);

		// Getter-Funktionen fÃ¼r private Variablen
		HWND GetHWND();
		unsigned int GetAdapterIndex();
		unsigned int GetOutputIndex();
		unsigned int GetWidth();
		unsigned int GetHeight();
		unsigned int GetColorDepth();

		BufferManager& GetBM();			// BufferManager
		ObjectManager& GetOM();			// ObjectManager
		ShaderManager& GetSM();			// ShaderManager
		LightManager& GetLM();			// LightManager
		InputLayoutManager& GetILM();	// InputManager
		TextureManager& GetTM();		// TextureManager
		CameraManager& GetCam();		// KameraManager

		// Setter-Funktionen fÃ¼r private Variablen
		void SetAdapter(unsigned int index);
		void SetOutput(unsigned int index);
		void SetDirectionalLight(LPENTITY entity);
		void SetGlobalAmbient(const DirectX::XMFLOAT4& ambient) { m_globalAmbient = ambient; }
		DirectX::XMFLOAT4 GetGlobalAmbient() const { return m_globalAmbient; }

		void SetCamera(LPENTITY mesh);

	private:
		static bool running;
		static double deltaTime;
		static double accumulator;
		static std::chrono::high_resolution_clock::time_point lastFrameTime;

		static constexpr double FIXED_TIMESTEP = 1.0 / 60.0;
		static constexpr double TARGET_FPS = 60.0;

		void UpdateFrameTime();

	public:

		//double GetDeltaTime() { return Timer::GetDeltaTime(); }

	};
}