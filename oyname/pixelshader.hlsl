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

Texture2D textureMap : register(t0);
SamplerState samplerState : register(s0);

Texture2D shadowMapTexture : register(t1); 
SamplerState shadowSampler : register(s1); 

float4 main(PS_INPUT input) : SV_Target
{
    // Definiere die Richtung des Lichts (normiert)
    float3 lightDirection = normalize(input.lightDirection);
    
    float ka = 0.5; // Materialreflexionskoeffizient
    
    // Beleuchtungseigenschaften
    float3 ia = input.lightColor;
    float3 id = float3(0.8, 1.0, 0.8); // Diffuse Lichtfarbe
    
    // Berechne den Diffuslichtbeitrag
    float diffuse_factor = max(dot(input.normal, -lightDirection), 0.0);
    float3 diffuse_light = id * diffuse_factor;
    
    // Berechne Umgebungslichtbeitrag
    float3 ambient_light = ka * ia;
    
    // Texturkoordinaten abrufen
    float2 texCoord = input.texCoord;
    
    // Texturfarbe abrufen
    float4 texColor = textureMap.Sample(samplerState, texCoord);
    
    // Überprüfe, ob die Textur befüllt wurde
    bool textureFilled = (texColor.a > 0.0);
    
    // Berechne die endgültige Farbe unter Berücksichtigung der Textur, falls befüllt, sonst ohne Textur
    float3 final_color;
    
    // Schattenfaktor berechnen
    float shadowDepth = shadowMapTexture.SampleLevel(shadowSampler, input.texCoord, 0).r;
    float shadowFactor = (shadowDepth < input.lightPosition.z) ? 0.1f : 1.0f;
    
    if (textureFilled)
    {
        final_color = texColor.rgb * (ambient_light + diffuse_light) * input.color.rgb;// * shadowFactor;
    }
    else
    {
        final_color = (ambient_light + diffuse_light) * input.color.rgb;// * shadowFactor;
    }

    return float4(final_color, texColor.a);
}





