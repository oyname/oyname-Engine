// Shader.h
#pragma once
#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include "gdxutil.h"
#include "Material.h"

/// <summary>
/// Shader-Klasse - Verwaltet Vertex und Pixel Shader mit Blobs und Input Layout
/// 
/// Memory Management:
/// - Destruktor gibt alle COM-Objekte frei (SafeRelease)
/// - Blobs werden nur für Input Layout Creation gebraucht, danach optional freigeben
/// 
/// Verwendung:
/// 1. ShaderManager::CreateShader() erstellt Shader-Objekt
/// 2. InputLayout wird mit Blobs erstellt
/// 3. UpdateShader() setzt den Shader als aktiv
/// </summary>

// Pass-aware Binding (Vorbereitung für Multi-Pass: Shadow/Depth, GBuffer, etc.)
enum class ShaderBindMode
{
    VS_PS,   // Standard: Vertex + Pixel
    VS_ONLY  // Depth/Shadow: nur Vertex, PixelShader wird auf nullptr gesetzt
};

class Shader {
public:
    // ==================== KONSTRUKTOR / DESTRUKTOR ====================
    Shader();
    ~Shader();

    // ==================== SHADER STATE ====================
    /// <summary>Flag ob dieser Shader gerade aktiv ist</summary>
    bool isActive;

    // ==================== VERTEX FORMAT INFORMATION ====================
    /// <summary>
    /// Vertex Format Flags (Bitwise kombiniert)
    /// z.B. D3DVERTEX_POSITION | D3DVERTEX_COLOR | D3DVERTEX_NORMAL
    /// Wird für Input Layout Creation genutzt
    /// </summary>
    DWORD flagsVertex;

    // ==================== SHADER-DATEIEN ====================
    /// <summary>Pfad zur Vertex Shader Datei (.hlsl)</summary>
    std::wstring vertexShaderFile;
    /// <summary>Pfad zur Pixel Shader Datei (.hlsl)</summary>
    std::wstring pixelShaderFile;

    // ==================== DIRECTX SHADER OBJEKTE ====================
    /// <summary>Input Layout - definiert Vertex-Struktur für diesen Shader</summary>
    ID3D11InputLayout* inputlayoutVertex;
    /// <summary>Compiled Vertex Shader</summary>
    ID3D11VertexShader* vertexShader;
    /// <summary>Compiled Pixel Shader</summary>
    ID3D11PixelShader* pixelShader;

    // ==================== SHADER BLOBS (für Debugging/Recompile) ====================
    /// <summary>
    /// Vertex Shader Bytecode Blob
    /// Wird benötigt für:
    /// - Input Layout Creation
    /// - Debugging
    /// - Shader Recompilation
    /// 
    /// Kann nach Input Layout Creation freigegeben werden:
    /// if (blobVS) { blobVS->Release(); blobVS = nullptr; }
    /// </summary>
    ID3D10Blob* blobVS;
    /// <summary>
    /// Pixel Shader Bytecode Blob
    /// Wird hauptsächlich für Debugging genutzt
    /// Kann nach Shader-Erstellung freigegeben werden
    /// </summary>
    ID3D10Blob* blobPS;

    // ==================== MATERIAL-VERWALTUNG ====================
    /// <summary>Vector aller Materials die diesen Shader nutzen</summary>
    std::vector<Material*> materials;

    // ==================== ÖFFENTLICHE METHODEN ====================
    /// <summary>
    /// Setzt diesen Shader als aktiv (Input Layout, VS, optional PS)
    /// 
    /// mode:
    /// - VS_PS: InputLayout + VS + PS
    /// - VS_ONLY: InputLayout + VS, PS wird auf nullptr gesetzt (Depth/Shadow Pass)
    /// 
    /// ⚠️ Fehlerbehandlung: Prüft auf nullptr Pointers
    /// </summary>
    void UpdateShader(const gdx::CDevice* device, ShaderBindMode mode = ShaderBindMode::VS_PS);

    // ==================== HILFSMETHODEN ====================
    /// <summary>
    /// Gibt Vertex Shader Bytecode zurück (aus Blob)
    /// Nützlich für Input Layout Creation
    /// </summary>
    inline void* GetVertexBytecode() const {
        return blobVS ? blobVS->GetBufferPointer() : nullptr;
    }
    /// <summary>
    /// Gibt Größe des Vertex Shader Bytecode zurück
    /// Nützlich für Input Layout Creation
    /// </summary>
    inline SIZE_T GetVertexBytecodeSize() const {
        return blobVS ? blobVS->GetBufferSize() : 0;
    }

    /// <summary>
    /// Gibt an ob dieser Shader vollständig initialisiert ist – abhängig vom Bind-Mode
    /// </summary>
    inline bool IsValid(ShaderBindMode mode) const
    {
        // Grundanforderungen: Layout + VS müssen immer da sein
        if (vertexShader == nullptr || inputlayoutVertex == nullptr)
            return false;

        // Optional: Wenn du blobVS zwingend brauchst, hier prüfen
        // if (blobVS == nullptr) return false;

        if (mode == ShaderBindMode::VS_PS)
        {
            if (pixelShader == nullptr)
                return false;
            // Optional: blobPS checken, wenn du es wirklich brauchst
            // if (blobPS == nullptr) return false;
        }

        return true;
    }

    /// <summary>
    /// Backwards compatible: "vollständig" = normaler Renderpass (VS+PS)
    /// </summary>
    inline bool IsValid() const {
        return IsValid(ShaderBindMode::VS_PS);
    }
};

typedef Shader* LPSHADER;
typedef Shader SHADER;