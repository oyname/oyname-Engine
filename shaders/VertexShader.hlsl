// ==================== CONSTANT BUFFERS ====================

cbuffer ConstantBuffer : register(b0)
{
    row_major float4x4 _viewMatrix;
    row_major float4x4 _projectionMatrix;
    row_major float4x4 _worldMatrix;
};

// Struktur für ein einzelnes Licht (muss mit C++ LightBufferData kompatibel sein!)
struct LightData
{
    float4 lightPosition; // XYZ: Position, W: 0 für direktional, 1 für positional
    float4 lightDirection; // XYZ: Direction
    float4 lightDiffuseColor; // RGB: Diffuse Farbe
    float4 lightAmbientColor; // RGB: Ambient Farbe (nur für erstes Licht relevant)
};

// Light-Array Buffer (SYNCHRON mit Pixel-Shader)
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

// ==================== PHASE 2: SHADOW MAPPING BUFFER ====================

cbuffer ShadowMatrixBuffer : register(b3)
{
    row_major float4x4 lightViewMatrix;
    row_major float4x4 lightProjectionMatrix;
};

// ==================== INPUT / OUTPUT STRUCTURES ====================

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION; // Bildschirmraumposition
    float3 normal : NORMAL; // Normale im Welt-Raum
    float3 worldPosition : TEXCOORD1; // Position im Welt-Raum (für Point-Light-Berechnung)
    float4 color : COLOR; // Farbe des Vertex
    float2 texCoord : TEXCOORD0; // Texturkoordinaten
    float4 positionLightSpace : TEXCOORD2; // Position im Licht-Raum (PHASE 2: Shadow Mapping)
};

// ==================== MAIN VERTEX SHADER ====================

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT o;

    // Transformiere die Position des Vertices mit World-, View- und Projektionsmatrix
    float4 tempPosition = float4(input.position, 1.0f);
    float4 worldPos = mul(tempPosition, _worldMatrix);
    o.worldPosition = worldPos.xyz; // Speichere Welt-Position für PS
    
    o.position = mul(worldPos, _viewMatrix);
    o.position = mul(o.position, _projectionMatrix);

    // Transformiere die Normale in den Welt-Raum
    // gdxdevice.cpp: Correctly apply world matrix to normal (without translation)
    o.normal = normalize(mul(input.normal, (float3x3) _worldMatrix));

    // Kopiere Vertex-Attribute
    o.color = input.color;
    o.texCoord = input.texCoord;
    
    // ==================== PHASE 2: BERECHNE LIGHT SPACE POSITION ====================
    // Transformiere Welt-Position in Light-Space für Shadow Mapping
    float4 lightViewPos = mul(worldPos, lightViewMatrix);
    o.positionLightSpace = mul(lightViewPos, lightProjectionMatrix);
    
    return o;
}

