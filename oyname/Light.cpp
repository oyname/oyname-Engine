#include "Light.h"

Light::Light():lightBuffer(nullptr)
{
	type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;

	DirectX::XMStoreFloat4(&cbLight.lightPosition, DirectX::XMVECTOR{0.0f,0.0f,0.0f,0.0f});
	DirectX::XMStoreFloat4(&cbLight.lightDirection, DirectX::XMVECTOR{0.0f,1.0f,0.0f,0.0f});

	SetDiffuseColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
}

Light::~Light()
{
	Memory::SafeRelease(lightBuffer);
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
void Light::UpdateLight(const gdx::CDevice* device, XMVECTOR position, XMVECTOR lookAt)
{
	HRESULT hr = S_OK;

	// Aktualisieren der Position des Richtungslichts
	DirectX::XMStoreFloat4(&cbLight.lightPosition, position);
	DirectX::XMStoreFloat4(&cbLight.lightDirection, lookAt);

	// Kopieren der Daten in den lightBuffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = device->GetDeviceContext()->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr))) {
		return;
	}

	memcpy(mappedResource.pData, &cbLight, sizeof(LightBufferData));
	device->GetDeviceContext()->Unmap(lightBuffer, 0);

	// Setzen des lightBuffers im Shader
	device->GetDeviceContext()->VSSetConstantBuffers(1, 1, &lightBuffer);
	device->GetDeviceContext()->PSSetConstantBuffers(1, 1, &lightBuffer);
}

