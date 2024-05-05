#include "Camera.h"

Camera::Camera()
{
    viewport = { 0 };

    matrixSet.projectionMatrix = DirectX::XMMatrixIdentity();
    matrixSet.viewMatrix = DirectX::XMMatrixIdentity();
    matrixSet.worldMatrix = DirectX::XMMatrixIdentity();
}

Camera::~Camera()
{

}

void Camera::GenerateViewMatrix(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt, DirectX::XMVECTOR up)
{
	this->matrixSet.viewMatrix = DirectX::XMMatrixLookAtLH(position, lookAt, up);
}

void Camera::GenerateProjectionMatrix(float fieldOfView, float screenAspect, float nearZ, float farZ)
{
	this->matrixSet.projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, nearZ, farZ);
}

void Camera::GenerateViewport(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth)
{
    viewport.Width = Width;
    viewport.Height = Height;
    viewport.MinDepth = MinDepth;
    viewport.MaxDepth = MaxDepth;
    viewport.TopLeftX = TopLeftX;
    viewport.TopLeftY = TopLeftY;
}
