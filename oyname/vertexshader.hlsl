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
};

struct VS_INPUT
{
    float4 position : POSITION;
    float3 normal : NORMAL;   
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 normal : TEXCOORD0;
    float4 lightPosition : TEXCOORD1;
    float3 lightColor : TEXCOORD2;
    float3 lightDirection : TEXCOORD3;
};

// Erster Vertex-Shader (main)
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT o;

    // Transformiere die Position des Vertices mit der Worldmatrix, Viewmatrix und der Projektionsmatrix
    o.position = mul(input.position, _worldMatrix);
    o.position = mul(o.position, _viewMatrix);
    o.position = mul(o.position, _projectionMatrix);

    o.lightPosition = lightPosition;
    o.lightDirection = lightDirection;
    o.lightColor = lightColor.rgb;
    
    // Transformiere die Normale in den Welt-Raum
    o.normal = normalize(mul(input.normal, (float3x3) _worldMatrix));

    // Kopiere die Farbe einfach
    o.color = input.color; // Diffuse Lichtfarbe input.color;

    return o;
}
