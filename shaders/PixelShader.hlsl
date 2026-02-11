// ==================== CONSTANT BUFFERS ====================

// Struktur für ein einzelnes Licht (muss mit C++ LightBufferData kompatibel sein!)
struct LightData
{
    float4 lightPosition; // XYZ: Position, W: 0 für direktional, 1 für positional
    float4 lightDirection; // XYZ: Direction (nur für direktionale Lichter relevant)
    float4 lightDiffuseColor; // RGB: Diffuse Farbe, A: Radius (für Point-Lights)
    float4 lightAmbientColor; // RGB: Ambient Farbe (nur für erstes Licht relevant)
};

// Light-Array Buffer
cbuffer LightBuffer : register(b1)
{
    LightData lights[32]; // Array von bis zu 32 Lichtern
    uint lightCount; // Aktuelle Anzahl der Lichter
    float3 lightPadding; // Padding für 16-Byte Alignment
};

cbuffer MaterialBuffer : register(b2)
{
    float4 diffuseColor;
    float4 specularColor;
    float shininess;
    float transparency;
    float2 padding;
};

// ==================== INPUT STRUCTURE ====================

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1; // Welt-Position des Pixels
    float4 color : COLOR;
    float2 texCoord : TEXCOORD0;
    float4 positionLightSpace : TEXCOORD2;
};

// ==================== TEXTURES & SAMPLERS ====================

Texture2D textureMap : register(t0);
SamplerState samplerState : register(s0);

Texture2D shadowMapTexture : register(t1);
SamplerState shadowSampler : register(s1);

// ==================== HELPER FUNCTIONS ====================

// Berechnet den Falloff für Point-Lights
// distance: Entfernung zum Licht
// radius: Maximale Reichweite des Lichts
float CalculateLightFalloff(float distance, float radius)
{
    // Lineare Falloff: 1 bei distance=0, 0 bei distance=radius
    float attenuation = max(0.0f, 1.0f - (distance / radius));
    return attenuation * attenuation; // Quadratisch für realistischere Abnahme
}

// ==================== MAIN PIXEL SHADER ====================

float4 main(PS_INPUT input) : SV_Target
{
    // Normalisiere Normal einmal (wird für alle Lichter genutzt)
    float3 normal = normalize(input.normal);
    
    // gdxdevice.cpp: Kamera-Position für Specula wird später benötigt (zukünftige Erweiterung)
    // Für jetzt: Vereinfachte Specula ohne View-Direction
    
    // ═══════════════════════════════════════════════════════════════════════════
    // BELEUCHTUNG: Ambient nur vom ersten Licht
    // ═══════════════════════════════════════════════════════════════════════════
    
    float3 ambient = float3(0.0, 0.0, 0.0);
    float3 diffuseAccum = float3(0.0, 0.0, 0.0);
    float3 specularAccum = float3(0.0, 0.0, 0.0);
    
    // Ambient vom ersten Licht
    if (lightCount > 0)
    {
        ambient = lights[0].lightAmbientColor.rgb;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // ALLE LICHTER DURCHLAUFEN: Diffuse und Specula akkumulieren
    // ═══════════════════════════════════════════════════════════════════════════
    
    for (uint i = 0; i < lightCount; ++i)
    {
        float3 lightDir;
        float lightIntensity = 1.0f;
        
        // Unterscheide zwischen direktionalen und positionale Lichtern
        // W-Komponente: 0 = direktional, 1 = positional
        if (lights[i].lightPosition.w > 0.5f)
        {
            // PUNKT-LICHT: Berechne Richtung und Falloff
            float3 lightToPixel = input.worldPosition - lights[i].lightPosition.xyz;
            float distance = length(lightToPixel);
            
            lightDir = normalize(lightToPixel); // Richtung vom Licht zum Surface
            
            // Radius aus der Alpha-Komponente der DiffuseColor (oder Default)
            float radius = lights[i].lightDiffuseColor.a > 0.1f ? lights[i].lightDiffuseColor.a : 100.0f;
            lightIntensity = CalculateLightFalloff(distance, radius);
        }
        else
        {
            // DIREKTIONALES LICHT: Nutze Direction direkt
            lightDir = normalize(lights[i].lightDirection.xyz);
        }
        
        // ───────────────────────────────────────────────────────────────────
        // DIFFUSE: Lambert-Kosinus-Gesetz
        // ───────────────────────────────────────────────────────────────────
        // gdxdevice.cpp: Korrekte Licht-Richtung (vom Surface zur Lichtquelle)
        float diffuse_factor = max(dot(normal, -lightDir), 0.0f);
        diffuseAccum += lights[i].lightDiffuseColor.rgb * diffuse_factor * lightIntensity;
        
        // ───────────────────────────────────────────────────────────────────
        // SPECULA: Blinn-Phong Modell
        // ───────────────────────────────────────────────────────────────────
        // Vereinfachte Version ohne View-Direction (zukünftig von VS)
        // Verwendet Half-Vector aus Licht-Richtung und Oberflächennormal
        float3 halfVec = normalize(-lightDir + float3(0, 1, 0)); // Placeholder für View-Direction
        float specular_factor = pow(max(dot(normal, halfVec), 0.0f), shininess);
        specularAccum += specularColor.rgb * specular_factor * lightIntensity;
    }
    
    // Gesamtbeleuchtung = Ambient + akkumulierte Diffuse + akkumulierte Specula
    float3 lighting = ambient + diffuseAccum + specularAccum;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // MATERIAL & TEXTUR
    // ═══════════════════════════════════════════════════════════════════════════
    
    // Texturfarbe abrufen
    float4 texColor = textureMap.Sample(samplerState, input.texCoord);
    
    // Prüfe ob Material-Farbe gesetzt ist (nicht schwarz)
    bool hasMaterialColor = (diffuseColor.r > 0.01 || diffuseColor.g > 0.01 || diffuseColor.b > 0.01);
    
    // Standard Material-Farbe falls nicht gesetzt
    float3 matColor = hasMaterialColor ? diffuseColor.rgb : float3(1.0, 1.0, 1.0);
    
    // Finale Farbe berechnen
    float3 final_color;
    
    if (texColor.a > 0.0)
    {
        // Mit Textur: Kombiniere Textur mit Material und Vertex-Farbe
        final_color = texColor.rgb * matColor * lighting * input.color.rgb;
    }
    else
    {
        // Ohne Textur: Nutze Material-Farbe mit Vertex-Farbe
        final_color = matColor * lighting * input.color.rgb;
    }
    
    // Finale Alpha aus Material (mit Fallback)
    float finalAlpha = hasMaterialColor ? (diffuseColor.a * transparency) : 1.0;
    
    return float4(final_color, finalAlpha);
}
