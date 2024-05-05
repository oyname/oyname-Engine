#pragma once

#include "gdxutil.h"

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

public:
	Camera();
	~Camera();

	MatrixSet matrixSet;
	D3D11_VIEWPORT viewport;

	void GenerateViewport(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth);
	void GenerateViewMatrix(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt, DirectX::XMVECTOR up);
	void GenerateProjectionMatrix(float fieldOfView, float screenAspect, float nearZ, float farZ);

	DirectX::XMMATRIX const GetViewMatrix(DirectX::XMMATRIX&) const {
		return matrixSet.viewMatrix;
	}
	DirectX::XMMATRIX const GetProjectionMatrix(DirectX::XMMATRIX&) const {
		return matrixSet.projectionMatrix;
	}

private:
};

