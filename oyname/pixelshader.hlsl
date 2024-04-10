struct PS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float4 lightposition : TEXCOORD0;
    float3 lightcolor : TEXCOORD1;
};

// Hauptfunktion des Pixel-Shaders
float4 main(PS_INPUT input) : SV_Target
{
    // Definiere die Position des Lichts
    float3 lightPosition = input.lightposition.xyz;

    float ka = 0.3; // Materialreflexionskoeffizient

    // Beleuchtungseigenschaften
    float3 ia = input.lightcolor;// Umgebungslichtfarbe
    float3 id = float3(1.0, 1.0, 1.0); // Diffuse Lichtfarbe

    float3 lightDirection = normalize(lightPosition - input.position);
    
    // Berechne den Diffuslichtbeitrag
    float diffuse_factor = max(dot(input.normal, lightDirection), 0.0);
    float3 diffuse_light = id * diffuse_factor;

    // Berechne Umgebungslichtbeitrag
    float3 ambient_light = ka * ia;

    // Berechne die endgültige Farbe
    float3 final_color = input.color.rgb * (ambient_light + diffuse_light);

    // Gib die Farbe als Ausgabe zurück
    return float4(final_color, input.color.a);
}

// Hauptfunktion des Pixel-Shaders
float4 main2(PS_INPUT input) : SV_Target
{
    // Definiere die Position des Lichts
    float3 lightPosition = input.lightposition.xyz;

    float ka = 0.2; // Materialreflexionskoeffizient

    // Beleuchtungseigenschaften
    float3 ia = float3(1.0, 1.0, 1.0); // Umgebungslichtfarbe
    float3 id = float3(1.0, 1.0, 1.0); // Diffuse Lichtfarbe

    float3 lightDirection = normalize(lightPosition - input.position);
    
    // Berechne den Diffuslichtbeitrag
    float diffuse_factor = max(dot(input.normal, lightDirection), 0.0);
    float3 diffuse_light = id * diffuse_factor;

    // Berechne Umgebungslichtbeitrag
    float3 ambient_light = ka * ia;

    // Berechne die endgültige Farbe
    float3 final_color =  (ambient_light + diffuse_light);

    // Gib die Farbe als Ausgabe zurück
    return float4(final_color, input.color.a);
}





