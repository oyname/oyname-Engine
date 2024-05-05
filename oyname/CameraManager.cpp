#include "CameraManager.h"

namespace gdx
{
    CameraManager::CameraManager() : m_currentCam(nullptr)
    { }

    CameraManager::~CameraManager()
    { }

    void CameraManager::SetCamera(LPENTITY camera)
    {
        m_currentCam = camera;
    }
}