struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;    
    float4 color : COLOR;
    float2 texCoord : TEXCOORD0;   
    float4 lightPosition : TEXCOORD1;
    float3 lightColor : TEXCOORD2;
    float3 lightDirection : TEXCOORD3;
};

Texture2D textureMap; // Hinzugefügt: Die Textur, die Sie verwenden möchten
SamplerState samplerState;

// Hauptfunktion des Pixel-Shaders
float4 main(PS_INPUT input) : SV_Target
{
    // Definiere die Richtung des Lichts (normiert)
    float3 lightDirection = normalize(input.lightDirection);
    
    float ka = 0.2; // Materialreflexionskoeffizient
    
    // Beleuchtungseigenschaften
    float3 ia = input.lightColor; // Umgebungslichtfarbe
    float3 id = float3(1.0, 1.0, 1.0); // Diffuse Lichtfarbe
    
    // Berechne den Diffuslichtbeitrag
    float diffuse_factor = max(dot(input.normal, -lightDirection), 0.0);
    float3 diffuse_light = id * diffuse_factor;
    
    // Berechne Umgebungslichtbeitrag
    float3 ambient_light = ka * ia;
    
    // Texturkoordinaten abrufen
    float2 texCoord = input.texCoord;
    
    // Texturfarbe abrufen
    float4 texColor = textureMap.Sample(samplerState, texCoord);
    
    // Berechne die endgültige Farbe unter Berücksichtigung der Textur
    float3 final_color = texColor.rgb * (ambient_light + diffuse_light) * input.color.rgb;

    // Gib die Farbe als Ausgabe zurück

   // return textureMap.Sample(samplerState, input.texCoord);
    return float4(final_color, texColor.a);
    
    // Berechne die endgültige Farbe
    //float3 final_color = input.color.rgb * (ambient_light + diffuse_light);
    
    // Gib die Farbe als Ausgabe zurück
    //return float4(final_color, input.color.a);
}





