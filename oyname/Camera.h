#pragma once

#include "gdxutil.h"

using namespace DirectX;

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

	void UpdateCamera(XMVECTOR position, XMVECTOR lookAt, XMVECTOR up);

	void GenerateViewport(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth);
	void GenerateViewMatrix(XMVECTOR position, XMVECTOR lookAt, XMVECTOR up);
	void GenerateProjectionMatrix(float fieldOfView, float screenAspect, float nearZ, float farZ);

	DirectX::XMMATRIX const GetViewMatrix(XMMATRIX&) const {
		return matrixSet.viewMatrix;
	}
	DirectX::XMMATRIX const GetProjectionMatrix(XMMATRIX&) const {
		return matrixSet.projectionMatrix;
	}

private:
};

