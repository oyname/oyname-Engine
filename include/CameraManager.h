#pragma once

#include "gdxutil.h"
#include "Camera.h"  // ← Statt Entity.h

namespace gdx
{
    class CameraManager
    {
    public:
        CameraManager();
        ~CameraManager();

        void SetCamera(Camera* camera);  // ← Camera* statt LPENTITY

        Camera* GetCurrentCam() const {  // ← Camera* statt LPENTITY
            return m_currentCam;
        }

    private:
        Camera* m_currentCam;  // ← Camera* statt LPENTITY
    };
}