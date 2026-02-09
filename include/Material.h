#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include "Mesh.h"   

class Material
{
public:
    // ==================== MATERIAL DATA STRUCT ====================
    struct MaterialData {
        DirectX::XMFLOAT4 diffuseColor;
        DirectX::XMFLOAT4 specularColor;
        float shininess;
        float transparency;
        float padding[2];
    };

    // ==================== KONSTRUKTOR / DESTRUKTOR ====================
    Material();
    ~Material();

    // ==================== TEXTURE METHODS ====================
    void SetTexture(const gdx::CDevice* device);
    void SetTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureView, ID3D11SamplerState* imageSamplerState);
    void UpdateConstantBuffer(ID3D11DeviceContext* context);

    // ==================== MATERIAL PROPERTY SETTERS ====================
    void SetDiffuseColor(float r, float g, float b, float a = 1.0f);
    void SetSpecularColor(float r, float g, float b, float a = 1.0f);
    void SetShininess(float shininess);
    void SetTransparency(float transparency);
    void SetColor(float r, float g, float b, float a = 1.0f);  // Kurzform für Diffuse

    // ==================== MATERIAL PROPERTY GETTERS ====================
    DirectX::XMFLOAT4 GetDiffuseColor() const;
    DirectX::XMFLOAT4 GetSpecularColor() const;
    float GetShininess() const;
    float GetTransparency() const;

    // ==================== MATERIAL STATE ====================
    bool isActive;
    MaterialData properties;  // Alle Material-Properties hier!

    // ==================== TEXTURE DATA ====================
    ID3D11Texture2D* m_texture;
    ID3D11ShaderResourceView* m_textureView;
    ID3D11SamplerState* m_imageSamplerState;
    ID3D11Buffer* materialBuffer;  // Constant Buffer für GPU

    // ==================== OBJECT MANAGEMENT ====================
    /// <summary>Alle Meshes die dieses Material verwenden</summary>
    std::vector<Mesh*> meshes;
    /// <summary>Shader dieser Material</summary>
    void* pRenderShader;

    // ==================== MEMORY MANAGEMENT ====================
    void* operator new(size_t size) {
        return _aligned_malloc(size, 16);
    }
    void operator delete(void* p) noexcept {
        _aligned_free(p);
    }
};

typedef Material* LPMATERIAL;
typedef Material MATERIAL;
