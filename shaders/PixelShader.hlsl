// ==================== CONSTANT BUFFERS ====================
cbuffer LightBuffer : register(b1)
{
    float4 lightPosition;
    float4 lightDirection;
    float4 lightColor;
    float4 lightAmbientColor;
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
    float4 lightPosition : TEXCOORD2;
    float3 lightColor : TEXCOORD3;
    float3 lightDirection : TEXCOORD4;
};

// ==================== TEXTURES & SAMPLERS ====================
Texture2D textureMap : register(t0);
SamplerState samplerState : register(s0);

Texture2D shadowMapTexture : register(t1);
SamplerState shadowSampler : register(s1);

// ==================== MAIN PIXEL SHADER ====================
float4 main(PS_INPUT input) : SV_Target
{
    // Definiere die Richtung des Lichts (normiert)
    float3 lightDir = normalize(input.lightDirection);
    
    // Beleuchtungskoeffizienten
    float3 ambient = lightAmbientColor.rgb;
    
    // Berechne den Diffuslichtbeitrag
    float diffuse_factor = max(dot(normalize(input.normal), -lightDir), 0.0);
    float3 diffuse = input.lightColor * diffuse_factor;
    
    // Gesamtbeleuchtung
    float3 lighting = ambient + diffuse;
    
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