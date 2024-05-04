#include "Light.h"

Light::Light():lightBuffer(nullptr)
{
	type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;
	SetPosition(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	SetDirection(DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f));
	SetDiffuseColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
}

Light::~Light()
{
	Memory::SafeRelease(lightBuffer);
}

void Light::SetPosition(const DirectX::XMFLOAT4& newPosition)
{
	cbLight.lightPosition = newPosition;
}

void Light::SetPosition(const DirectX::XMVECTOR& newPosition)
{
	DirectX::XMStoreFloat4(&cbLight.lightPosition, newPosition);
}

void Light::SetDirection(const DirectX::XMFLOAT4& newDirection)
{
	cbLight.lightDirection = newDirection;
}

void Light::SetDirection(const DirectX::XMVECTOR& newDirection)
{
	DirectX::XMStoreFloat4(&cbLight.lightDirection, newDirection);
}

void Light::SetDiffuseColor(const DirectX::XMFLOAT4& Color)
{
	cbLight.lightDiffuseColor.x = Color.x;
	cbLight.lightDiffuseColor.y = Color.y;
	cbLight.lightDiffuseColor.z = Color.z;
	cbLight.lightDiffuseColor.w = Color.w;
}

void Light::SetAmbientColor(const DirectX::XMFLOAT4& Color)
{
	cbLight.lightAmbientColor.x = Color.x;
	cbLight.lightAmbientColor.y = Color.y;
	cbLight.lightAmbientColor.z = Color.z;
	cbLight.lightAmbientColor.w = Color.w;
}

void Light::SetLightType(D3DLIGHTTYPE lightType)
{
	this->type = lightType;
}

// Funktion zum Aktualisieren des Richtungslichts und Kopieren der Daten in den Shader-Buffer
void Light::UpdateLight(const gdx::CDevice* device)
{
	HRESULT hr = S_OK;

	// Aktualisieren der Position des Richtungslichts
	SetPosition(this->transform.getPosition());
	SetDirection(this->transform.getLookAt());

	// Kopieren der Daten in den lightBuffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = device->GetDeviceContext()->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
		return;
	}

	memcpy(mappedResource.pData, &cbLight, sizeof(LightSet));
	device->GetDeviceContext()->Unmap(lightBuffer, 0);

	// Setzen des lightBuffers im Shader
	device->GetDeviceContext()->VSSetConstantBuffers(1, 1, &lightBuffer);
	device->GetDeviceContext()->PSSetConstantBuffers(1, 1, &lightBuffer);
}

void Light::GenerateViewMatrix()
{
	this->cb.viewMatrix = DirectX::XMMatrixLookAtLH(this->transform.getPosition(), this->transform.getLookAt(), this->transform.getUp());
}

void Light::GenerateProjectionMatrix(float nearZ, float farZ)
{
	float fieldOfView, screenAspect;

	fieldOfView = 3.1415927f/2.0f;
	screenAspect = 1.0f;

	this->cb.projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, nearZ, farZ);
}
