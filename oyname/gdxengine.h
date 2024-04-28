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
#include "Camera.h"
#include "Transform.h"

#define VERTEX_SHADER_FILE L"VertexShader.hlsl"
#define PIXEL_SHADER_FILE L"PixelShader.hlsl"

namespace gdx {
	// MainLoop is used in gidx.cpp
	void MainLoop(bool running);

	bool MainLoop();

	// Shut down the main thread
	int ShutDown();

	class CGIDX {
	private:
		bool m_bInitialized;
		HWND m_hwnd;		   // Window handle
		int m_colorDepth;	   // Color depth (e.g., 32-bit colors)
		int m_screenWidth;	   // Screen width
		int m_screenHeight;	   // Screen height	
		int m_adapterIndex;	   // Current adapter index
		int m_monitorIndex;	   // Index of the monitor

		// Manager classes
		ObjectManager		m_objectManager;
		RenderManager		m_renderManager;
		ShaderManager		m_shaderManager;
		InputLayoutManager	m_inputLayoutManager;
		BufferManager		m_bufferManager;
		LightManager		m_lightManager;
		TextureManager		m_texturManager;
		Camera		        m_camera;

	public:
		CDevice m_device;		// Device Manager, not to be confused with DirectXDevice
		CInterface m_interface;	// Interface Manager
		
	public:
		CGIDX(HWND hwnd, HINSTANCE hinst, unsigned int bpp, unsigned int screenX, unsigned int screenY,  int* result);
		~CGIDX();

		void Cleanup();
		void RenderWorld();
		void UpdateWorld();

		// 
		HRESULT Graphic(unsigned int width, unsigned int height, bool windowed);
		HRESULT Cls(float r, float g, float b, float a);

		// Getter-Funktionen für private Variablen
		HWND GetHWND();
		unsigned int GetAdapterIndex();
		unsigned int GetOutputIndex();
		unsigned int GetWidth();
		unsigned int GetHeight();
		unsigned int GetColorDepth();

		BufferManager& GetBM();			// BufferManager
		ObjectManager&  GetOM();		// ObjectManager
		ShaderManager& GetSM();			// ShaderManager
		LightManager& GetLM();			// LightManager
		InputLayoutManager& GetILM();	// InputManager
		TextureManager& GetTM();		// TextureManager
		Camera& GetCam();				// Kamera
		
		// Setter-Funktionen für private Variablen
		void SetAdapter(unsigned int index);
		void SetOutput(unsigned int index);
		void SetCamera(LPMESH mesh);
	};
}
