#pragma once

#include "gdxutil.h"
#include "ObjectManager.h"

namespace gdx
{
	class CameraManager
	{
	public:
		CameraManager();
		~CameraManager();

		void SetCamera(LPENTITY mesh);

		const LPENTITY GetCurrentCam() const {
			return m_currentCam; 
		}

	private:

		LPENTITY m_currentCam;
	};
}

