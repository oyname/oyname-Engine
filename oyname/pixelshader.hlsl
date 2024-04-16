struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float4 lightPosition : TEXCOORD0;
    float3 lightColor : TEXCOORD1;
    float3 lightDirection : TEXCOORD2;
};

// Hauptfunktion des Pixel-Shaders
float4 main(PS_INPUT input) : SV_Target
{
    // Definiere die Richtung des Lichts (normiert)
    float3 lightDirection = normalize(input.lightDirection);
    
    float ka = 0.6; // Materialreflexionskoeffizient
    
    // Beleuchtungseigenschaften
    float3 ia = input.lightColor; // Umgebungslichtfarbe
    float3 id = float3(1.0, 1.0, 1.0); // Diffuse Lichtfarbe
    
    // Berechne den Diffuslichtbeitrag
    float diffuse_factor = max(dot(input.normal, -lightDirection), 0.0);
    float3 diffuse_light = id * diffuse_factor;
    
    // Berechne Umgebungslichtbeitrag
    float3 ambient_light = ka * ia;
    
    // Berechne die endgültige Farbe
    float3 final_color = input.color.rgb * (ambient_light + diffuse_light);
    
    // Gib die Farbe als Ausgabe zurück
    return float4(final_color, input.color.a);
}





