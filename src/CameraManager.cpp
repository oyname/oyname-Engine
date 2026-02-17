#include "CameraManager.h"


CameraManager::CameraManager() : m_currentCam(nullptr)
{
}

CameraManager::~CameraManager()
{
}

void CameraManager::SetCamera(Camera* camera)
{
    m_currentCam = camera;
}
