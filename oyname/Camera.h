#pragma once

#include "gdxutil.h"
#include "MeshManager.h"

namespace gdx 
{
	class Camera
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
		Camera();
		~Camera();

		void Init(gdx::CGIDX* engine);

		void SetViewport(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth);
		void SetPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
		void SetOrthographic(float width, float height, float nearPlane, float farPlane);
		void SetCamera(LPMESH mesh);

		LPMESH GetCurrentCam();

		void CreateCamera(LPMESH* camera);

		D3D11_VIEWPORT GetViewPort();

	private:
		gdx::CGIDX* m_engine;
		PROJ_MODE m_projMode;
		D3D11_VIEWPORT m_viewport;
		Perspective m_perspective;
		Ortho m_ortho;
		LPMESH m_currentCam;
	};
}