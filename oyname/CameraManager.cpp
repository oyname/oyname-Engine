#include "CameraManager.h"

namespace gdx
{
    CameraManager::CameraManager() : m_currentCam(nullptr), m_engine(nullptr)
    {
        m_projMode = PERSPECTIVE_LH;
        m_ortho = { 0 };
        m_viewport = { 0 };
        m_perspective = { 0 };
    }

    CameraManager::~CameraManager()
    { }

    void CameraManager::Init(gdx::CGIDX* engine)
    {
        m_projMode = PERSPECTIVE_LH;
        SetViewport(0, 0, 0, 0, 0, 0);
        SetPerspective(0.0f, 0.0f, 0.0f, 0.0f);
        SetOrthographic(0.0f, 0.0f, 0.0f, 0.0f);
        m_engine = engine;
    }

    void CameraManager::SetViewport(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth)
    {
        m_viewport.Width = Width;
        m_viewport.Height = Height;
        m_viewport.MinDepth = MinDepth;
        m_viewport.MaxDepth = MaxDepth;
        m_viewport.TopLeftX = TopLeftX;
        m_viewport.TopLeftY = TopLeftY;
    }

    void CameraManager::SetShadowMapViewport(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth)
    {
        m_shadowMapViewport.Width = Width;
        m_shadowMapViewport.Height = Height;
        m_shadowMapViewport.MinDepth = MinDepth;
        m_shadowMapViewport.MaxDepth = MaxDepth;
        m_shadowMapViewport.TopLeftX = TopLeftX;
        m_shadowMapViewport.TopLeftY = TopLeftY;
    }

    void CameraManager::SetPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
    {
        m_perspective.fieldOfView = fieldOfView;
        m_perspective.aspectRatio = aspectRatio;
        m_perspective.nearPlane = nearPlane;
        m_perspective.farPlane = farPlane;
    }

    void CameraManager::SetOrthographic(float width, float height, float nearPlane, float farPlane)
    {
        m_ortho.width = width;
        m_ortho.height = height;
        m_ortho.nearPlane = nearPlane;
        m_ortho.farPlane = farPlane;
    }

    void CameraManager::CreateCamera(LPENTITY* camera)
    {
        if (m_projMode == PERSPECTIVE_LH)
        {
            (*camera)->cb.projectionMatrix = XMMatrixPerspectiveFovLH(m_perspective.fieldOfView, m_perspective.aspectRatio, m_perspective.nearPlane, m_perspective.farPlane);
        }
        else if (m_projMode == ORTHO_LH)
        {
            (*camera)->cb.projectionMatrix = XMMatrixOrthographicLH(m_ortho.width, m_ortho.height, m_ortho.nearPlane, m_ortho.farPlane);
        }

        (*camera)->cb.viewMatrix = XMMatrixLookAtLH((*camera)->transform.getPosition(), (*camera)->transform.getLookAt(), (*camera)->transform.getUp());
        (*camera)->cb.worldMatrix = XMMatrixIdentity();
    }

    void CameraManager::SetCamera(LPENTITY camera)
    {
        m_currentCam = camera;
    }
}