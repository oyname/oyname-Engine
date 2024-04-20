#pragma once

#include <vector>
#include <string>
#include "gdxutil.h"

class Texture
{
public:
	Texture();
	~Texture();

	std::wstring m_sFilename;
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;
	ID3D11SamplerState* m_imageSamplerState;

	HRESULT AddTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename);
};

typedef Texture** LPLPTEXTURE;
typedef Texture* LPTEXTURE;
typedef Texture TEXTURE;

