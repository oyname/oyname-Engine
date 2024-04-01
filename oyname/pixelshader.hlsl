struct PS_INPUT
{
    float4 position: POSITION;
    float4 color: COLOR; // Test
};

// Hauptfunktion des Pixel-Shaders
float4 main(PS_INPUT input) : SV_Target{

    float4 color = input.color;

    return color;
}




