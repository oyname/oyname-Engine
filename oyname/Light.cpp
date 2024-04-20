#include "Light.h"

Light::Light():lightBuffer(nullptr)
{
	type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;
	SetPosition(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	SetDirection(DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f));
	SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
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

void Light::SetColor(const DirectX::XMFLOAT4& Color) 
{
	cbLight.lightColor.x = Color.x;
	cbLight.lightColor.y = Color.y;
	cbLight.lightColor.z = Color.z;
	cbLight.lightColor.w = Color.w;
}

DirectX::XMFLOAT4 Light::GetPosition() const
{
	DirectX::XMFLOAT4 positionFloat;
	DirectX::XMStoreFloat4(&positionFloat, this->position);
	return positionFloat;
}

DirectX::XMFLOAT4 Light::GetDirection() const
{
	DirectX::XMFLOAT4 directionFloat;
	DirectX::XMStoreFloat4(&directionFloat, this->lookAt);
	return directionFloat;
}

DirectX::XMFLOAT4 Light::GetColor() const
{ 
	return cbLight.lightColor;
}

// Funktion zum Aktualisieren des Richtungslichts und Kopieren der Daten in den Shader-Buffer
void Light::UpdateLight(const gdx::CDevice* device)
{
	HRESULT hr = S_OK;

	// Aktualisieren der Position des Richtungslichts
	SetPosition(position);
	SetDirection(lookAt);

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
