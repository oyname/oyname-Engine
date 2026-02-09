cbuffer ConstantBuffer : register(b0)
{
    row_major float4x4 _viewMatrix;
    row_major float4x4 _projectionMatrix;
    row_major float4x4 _worldMatrix;
};

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
    float3 normal : NORMAL; // Normale des Vertex im Welt-Raum
    float4 color : COLOR; // Farbe des Vertex
    float2 texCoord : TEXCOORD0; // Texturkoordinaten des Vertex
    float4 positionLightSpace : TEXCOORD1; // Position des Vertex im Licht-Raum (für Shadow Mapping)
    float4 lightPosition : TEXCOORD2; // Position des Lichts
    float3 lightColor : TEXCOORD3; // Farbe des Lichts
    float3 lightDirection : TEXCOORD4; // Richtung des Lichts
};

// Erster Vertex-Shader (main)
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT o;

    // Transformiere die Position des Vertices mit der Worldmatrix, Viewmatrix und der Projektionsmatrix
    float4 tempPosition = float4(input.position, 1.0f);
    o.position = mul(tempPosition, _worldMatrix);
    o.position = mul(o.position, _viewMatrix);
    o.position = mul(o.position, _projectionMatrix);

    o.lightPosition = lightPosition;
    o.lightDirection = lightDirection.xyz;
    o.lightColor = lightColor.rgb;
    
    // Transformiere die Normale in den Welt-Raum
    o.normal = normalize(mul(input.normal, (float3x3) _worldMatrix));

    // Kopiere die Farbe einfach
    o.color = input.color;

    // Kopiere die Texturkoordinaten
    o.texCoord = input.texCoord;
    
    return o;
}

