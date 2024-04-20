#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() : m_texture (nullptr), m_textureView (nullptr)
{
    m_sFilename = L"";
}

Texture::~Texture()
{

}

HRESULT Texture::AddTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename)
{
    // Bilddaten laden
    int imageWidth, imageHeight, imageChannels;
    int desiredChannels = 4;

    char* narrowFilename = nullptr;
    size_t length = wcslen(filename) + 1; // +1 für das Nullzeichen am Ende
    narrowFilename = new char[length];
    size_t convertedChars = 0;
    wcstombs_s(&convertedChars, narrowFilename, length, filename, length);
    unsigned char* imageData = stbi_load(narrowFilename, &imageWidth, &imageHeight, &imageChannels, desiredChannels);
    delete[] narrowFilename;

    if (!imageData)
    {
        Debug::Log("FAIL LOAD IMAGE ", __FILE__, __LINE__);
        return E_FAIL; // Fehler beim Laden der Bilddatei
    }

    // Berechnung der maximalen Größe
    UINT maxSize = imageWidth > imageHeight ? imageWidth : imageHeight;

    // Texturbeschreibung erstellen
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = imageWidth;
    textureDesc.Height = imageHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Subressource-Daten einrichten
    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = imageData;
    subresourceData.SysMemPitch = imageWidth * 4;

    // Textur erstellen
    ID3D11Texture2D* texture;
    HRESULT hr = device->CreateTexture2D(&textureDesc, &subresourceData, &texture);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        stbi_image_free(imageData); // Speicher freigeben
        return hr; // Fehler beim Erstellen der Textur
    }

    // Shader Resource View erstellen
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    ID3D11ShaderResourceView* srv;
    hr = device->CreateShaderResourceView(texture, &srvDesc, &srv);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        texture->Release(); // Textur freigeben
        stbi_image_free(imageData); // Speicher freigeben
        return hr; // Fehler beim Erstellen der Shader Resource View
    }

    D3D11_SAMPLER_DESC ImageSamplerDesc = {};

    ImageSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    ImageSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    ImageSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    ImageSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    ImageSamplerDesc.MipLODBias = 0.0f;
    ImageSamplerDesc.MaxAnisotropy = 1;
    ImageSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    ImageSamplerDesc.BorderColor[0] = 1.0f;
    ImageSamplerDesc.BorderColor[1] = 1.0f;
    ImageSamplerDesc.BorderColor[2] = 1.0f;
    ImageSamplerDesc.BorderColor[3] = 1.0f;
    ImageSamplerDesc.MinLOD = -FLT_MAX;
    ImageSamplerDesc.MaxLOD = FLT_MAX;

    hr = device->CreateSamplerState(&ImageSamplerDesc, &m_imageSamplerState);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        texture->Release(); // Textur freigeben
        stbi_image_free(imageData); // Speicher freigeben
        return hr; // Fehler beim Erstellen der Shader Resource View
    }

    // Speicher freigeben und Texture-Struktur zurückgeben
    stbi_image_free(imageData); // Bilddaten freigeben

    m_sFilename = filename;
    m_texture = texture;
    m_textureView = srv;

    return S_OK; // Erfolg

}