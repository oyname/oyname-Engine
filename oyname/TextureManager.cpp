#include "TextureManager.h"

using namespace DirectX;

gdx::TextureManager::TextureManager() {}

gdx::TextureManager::~TextureManager() {
    this->ReleaseTexture();
}

gdx::TextureManager& gdx::TextureManager::instance() {
    static TextureManager manager_;
    return manager_;
}

void gdx::TextureManager::ReleaseTexture() {
    for (auto& texture : this->tc) {
        Memory::SafeRelease(texture->m_texture);
        Memory::SafeRelease(texture->m_textureView);
        Memory::SafeDelete(texture);
    }
    this->tc.clear();
}

HRESULT gdx::TextureManager::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename, LPLPTEXTURE lpTexture) {
    HRESULT hr = S_OK;
    
    (*lpTexture) = NULL;

    int textureIndex = this->CheckTexture(filename);

    if (textureIndex == -1) 
    {
        (*lpTexture) = new TEXTURE;
        (*lpTexture)->AddTexture(device, deviceContext, filename);
        this->tc.push_back(*lpTexture);
    }
    else
    {
        (*lpTexture) = this->tc[textureIndex];
    }

    // Rückgabe der Textur
    return hr;
}

int gdx::TextureManager::CheckTexture(std::wstring sFilename) {
    int textureIndex = -1;

    for (size_t i = 0; i < this->tc.size(); ++i) {
        if (tc[i]->m_sFilename == sFilename) {

            textureIndex = static_cast<int>(i);
            break;
        }
    }

    return textureIndex;
}