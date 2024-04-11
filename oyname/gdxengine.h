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
#include "Camera.h"

namespace gdx {
	// MainLoop wird verwendet in gidx.cpp
	void MainLoop(bool running);

	bool MainLoop();

	// Zurück Main-Thread beeenden
	int ShutDown();

	class CGIDX {
	private:
		bool m_bInitialized;
		HWND m_hwnd;		   // Fensterhandle
		int m_colorDepth;	   // Farbtiefe (z.B. 32-Bit-Farben) 
		int m_screenWidth;	   // Bildschirmweite
		int m_screenHeight;	   // Bildschirmhöhe	
		int m_adapterIndex;	   // Aktueller Adapter
        int m_monitorIndex;	   // Index des Monitors

		// Manager-Klassen
		ObjectManager		m_objectManager;
		RenderManager		m_renderManager;
		ShaderManager		m_shaderManager;
		InputLayoutManager	m_inputLayoutManager;
		BufferManager		m_bufferManager;
		LightManager		m_lightManager;
		Camera		        m_camera;

	public:
		CDevice m_device;		// Device Manager nicht mit DirectXDevice verwechseln
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

		BufferManager& GetBM();
		ObjectManager&  GetOM();
		ShaderManager& GetSM();
		LightManager& GetLM();
		InputLayoutManager& GetILM();
		Camera& GetCam();
		
		// Setter-Funktionen für private Variablen
		void SetAdapter(unsigned int index);
		void SetOutput(unsigned int index);
		void SetFormat(DXGI_FORMAT format);
		void SetCamera(LPMESH mesh);
		void SetDirectionalLight(LPLIGHT dirLight);
	};
}
