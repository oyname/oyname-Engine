// ==================== CONSTANT BUFFERS ====================

// Struktur für ein einzelnes Licht (muss mit C++ LightBufferData kompatibel sein!)
struct LightData
{
    float4 lightPosition;       // XYZ: Position
    float4 lightDirection;      // XYZ: Direction
    float4 lightDiffuseColor;   // RGB: Diffuse Farbe (war lightColor im Original)
    float4 lightAmbientColor;   // RGB: Ambient Farbe (nur für erstes Licht relevant)
};

// Light-Array Buffer
cbuffer LightBuffer : register(b1)
{
    LightData lights[32];       // Array von bis zu 32 Lichtern
    uint lightCount;            // Aktuelle Anzahl der Lichter
    float3 lightPadding;        // Padding für 16-Byte Alignment
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
    float4 color : COLOR;
    float2 texCoord : TEXCOORD0;
    float4 positionLightSpace : TEXCOORD1;
    float4 lightPosition : TEXCOORD2;      // Noch kompatibel, aber nicht mehr genutzt
    float3 lightColor : TEXCOORD3;         // Noch kompatibel, aber nicht mehr genutzt
    float3 lightDirection : TEXCOORD4;     // Noch kompatibel, aber nicht mehr genutzt
};

// ==================== TEXTURES & SAMPLERS ====================
Texture2D textureMap : register(t0);
SamplerState samplerState : register(s0);

Texture2D shadowMapTexture : register(t1);
SamplerState shadowSampler : register(s1);

// ==================== MAIN PIXEL SHADER ====================
float4 main(PS_INPUT input) : SV_Target
{
    // Normalisiere Normal einmal (wird für alle Lichter genutzt)
    float3 normal = normalize(input.normal);
    
    // ═══════════════════════════════════════════════════════════════
    // BELEUCHTUNG: Ambient nur vom ersten Licht
    // ═══════════════════════════════════════════════════════════════
    float3 ambient = float3(0.0, 0.0, 0.0);
    float3 diffuseAccum = float3(0.0, 0.0, 0.0);
    
    // Ambient vom ersten Licht
    if (lightCount > 0)
    {
        ambient = lights[0].lightAmbientColor.rgb;
    }
    
    // ═══════════════════════════════════════════════════════════════
    // ALLE LICHTER DURCHLAUFEN: Diffuse akkumulieren
    // ═══════════════════════════════════════════════════════════════
    for (uint i = 0; i < lightCount; ++i)
    {
        // Lichts Richtung normalisieren
        float3 lightDir = normalize(lights[i].lightDirection.xyz);
        
        // Lambert-Kosinus-Gesetz (mit -lightDir wie im Original)
        float diffuse_factor = max(dot(normal, -lightDir), 0.0);
        
        // Diffuse für dieses Licht akkumulieren
        diffuseAccum += lights[i].lightDiffuseColor.rgb * diffuse_factor;
    }
    
    // Gesamtbeleuchtung = Ambient + akkumulierte Diffuse
    float3 lighting = ambient + diffuseAccum;
    
    // ═══════════════════════════════════════════════════════════════
    // MATERIAL & TEXTUR: Wie im Original
    // ═══════════════════════════════════════════════════════════════
    
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
