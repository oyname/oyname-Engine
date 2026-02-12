#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include "Mesh.h"

class Shader; // forward

class Material
{
public:
    // ==================== MATERIAL DATA STRUCT ====================
    // Must match PixelShader.hlsl cbuffer MaterialBuffer (b2)
    struct MaterialData {
        DirectX::XMFLOAT4 diffuseColor;
        DirectX::XMFLOAT4 specularColor;
        float shininess;
        float transparency;
        float receiveShadows; // 1.0 = receive, 0.0 = ignore
        float padding;        // 16-byte alignment
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
    void SetColor(float r, float g, float b, float a = 1.0f);  // Kurzform fuer Diffuse

    // ==================== MATERIAL PROPERTY GETTERS ====================
    DirectX::XMFLOAT4 GetDiffuseColor() const;
    DirectX::XMFLOAT4 GetSpecularColor() const;
    float GetShininess() const;
    float GetTransparency() const;

    // ==================== SHADOW FLAGS ====================
    // CPU-side flags
    bool castShadows = true;
    bool receiveShadows = true;

    inline void SetCastShadows(bool enabled) { castShadows = enabled; } // material->SetCastShadows(false);     // wirft keinen Schatten
    inline void SetReceiveShadows(bool enabled)                         // material->SetReceiveShadows(false);  // wird nicht abgedunkelt durch ShadowMap
    {
        receiveShadows = enabled;
        properties.receiveShadows = enabled ? 1.0f : 0.0f;
    }
    inline bool GetCastShadows() const { return castShadows; }
    inline bool GetReceiveShadows() const { return receiveShadows; }

    // ==================== MATERIAL STATE ====================
    bool isActive;
    MaterialData properties;  // Alle Material-Properties hier!

    // ==================== TEXTURE DATA ====================
    ID3D11Texture2D* m_texture;
    ID3D11ShaderResourceView* m_textureView;
    ID3D11SamplerState* m_imageSamplerState;
    ID3D11Buffer* materialBuffer;  // Constant Buffer fuer GPU

    // ==================== OBJECT MANAGEMENT ====================
    /// <summary>Alle Meshes die dieses Material verwenden</summary>
    std::vector<Mesh*> meshes;
    /// <summary>Shader dieses Materials (Main pass shader)</summary>
    Shader* pRenderShader;

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

