#pragma once

#include "gdxutil.h"
#include "ObjectManager.h"

namespace gdx
{
	class CameraManager
	{
		enum PROJ_MODE
		{
			ORTHO_LH,
			PERSPECTIVE_LH,
		};

		struct Perspective
		{
			float fieldOfView;
			float aspectRatio;
			float nearPlane;
			float farPlane;
		};

		struct Ortho
		{
			float width;
			float height;
			float nearPlane;
			float farPlane;
		};

	public:
		CameraManager();
		~CameraManager();

		void Init(gdx::CGIDX* engine);
		void CreateCamera(LPENTITY* camera);

		void SetViewport(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth);
		void SetShadowMapViewport(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth);
		void SetPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
		void SetOrthographic(float width, float height, float nearPlane, float farPlane);
		void SetCamera(LPENTITY mesh);

		const LPENTITY GetCurrentCam() const {
			return m_currentCam; 
		}
		D3D11_VIEWPORT* GetViewPort(){ 
			return &m_viewport; 
		}
		D3D11_VIEWPORT* GetViewPortShadow() {
			return &m_shadowMapViewport;
		}
		
	private:
		gdx::CGIDX* m_engine;
		PROJ_MODE m_projMode;
		D3D11_VIEWPORT m_viewport;
		D3D11_VIEWPORT m_shadowMapViewport;
		Perspective m_perspective;
		Ortho m_ortho;
		LPENTITY m_currentCam;
	};
}