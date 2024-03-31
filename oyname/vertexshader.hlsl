cbuffer ConstantBuffer : register(b0)
{
    row_major matrix viewMatrix;
    row_major matrix projectionMatrix;
    row_major matrix worldMatrix;
    // Weitere Konstanten hier definieren
};

struct VS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    // Transformiere die Position des Vertices mit der Worldmatrix, Viewmatrix und der Projektionsmatrix
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Kopiere die Farbe einfach
    output.color = input.color;

    return output;
}
