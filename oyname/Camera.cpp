#include "gdxengine.h"
#include "Camera.h"

namespace gdx
{
    Camera::Camera()
    {
    }

    Camera::~Camera()
    { }

    void Camera::Init(gdx::CGIDX* engine)
    {
        m_projMode = PERSPECTIVE_LH;
        SetViewport(0, 0, 0, 0, 0, 0);
        SetPerspective(0.0f, 0.0f, 0.0f, 0.0f);
        SetOrthographic(0.0f, 0.0f, 0.0f, 0.0f);
        m_engine = engine;
    }

    void Camera::SetViewport(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth)
    {
        m_viewport.TopLeftX = TopLeftX;
        m_viewport.TopLeftY = TopLeftY;
        m_viewport.Width = Width;
        m_viewport.Height = Height;
        m_viewport.MinDepth = MinDepth;
        m_viewport.MaxDepth = MaxDepth;
    }

    void Camera::SetPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
    {
        m_perspective.fieldOfView = fieldOfView;
        m_perspective.aspectRatio = aspectRatio;
        m_perspective.nearPlane = nearPlane;
        m_perspective.farPlane = farPlane;
    }

    void Camera::SetOrthographic(float width, float height, float nearPlane, float farPlane)
    {
        m_ortho.width = width;
        m_ortho.height = height;
        m_ortho.nearPlane = nearPlane;
        m_ortho.farPlane = farPlane;
    }

    void Camera::CreateCamera(LPMESH* camera)
    {
        // Kamera erstellen. Das macht der Objectmanager
        *camera = m_engine->GetMM().createMesh();

        // Perspektive
        if (m_projMode == PERSPECTIVE_LH)
        {
            (*camera)->cb.projectionMatrix = XMMatrixPerspectiveFovLH(m_perspective.fieldOfView, m_perspective.aspectRatio, m_perspective.nearPlane, m_perspective.farPlane);
        }
        else if (m_projMode == ORTHO_LH)
        {
            (*camera)->cb.projectionMatrix = XMMatrixOrthographicLH(m_ortho.width, m_ortho.height, m_ortho.nearPlane, m_ortho.farPlane);
        }

        (*camera)->cb.viewMatrix = XMMatrixLookAtLH((*camera)->position, (*camera)->lookAt, (*camera)->up);
        (*camera)->cb.worldMatrix = XMMatrixIdentity();
    }

    D3D11_VIEWPORT Camera::GetViewPort()
    {
        return m_viewport;
    }

    LPMESH Camera::GetCurrentCam()
    {
        return m_currentCam;
    }

    void Camera::SetCamera(LPMESH camera)
    {
        m_currentCam = camera;
    }
}