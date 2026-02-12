// ==================== CONSTANT BUFFERS ====================

// Struktur für ein einzelnes Licht (muss mit C++ LightBufferData kompatibel sein!)
struct LightData
{
    float4 lightPosition; // XYZ: Position, W: 0 für direktional, 1 für positional
    float4 lightDirection; // XYZ: Direction (nur für direktionale Lichter relevant)
    float4 lightDiffuseColor; // RGB: Diffuse Farbe, A: Radius (für Point-Lights)
    float4 lightAmbientColor; // RGB: Ambient Farbe (nur für erstes Licht relevant)
};

// Light-Array Buffer
cbuffer LightBuffer : register(b1)
{
    LightData lights[32];
    uint lightCount;
    float3 lightPadding;
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
    float3 worldPosition : TEXCOORD1;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD0;
    float4 positionLightSpace : TEXCOORD2;
};

// ==================== TEXTURES & SAMPLERS ====================

Texture2D textureMap : register(t0);
SamplerState samplerState : register(s0);

Texture2D shadowMapTexture : register(t1);
SamplerComparisonState shadowSampler : register(s1);

// ==================== HELPER FUNCTIONS ====================

// Point-Light Falloff
float CalculateLightFalloff(float distance, float radius)
{
    float attenuation = max(0.0f, 1.0f - (distance / radius));
    return attenuation * attenuation;
}

// Shadow PCF (3x3) für Directional Light
float ShadowPCF(float4 positionLightSpace, float3 normal, float3 lightDir /* lightDir = Richtung vom Licht */)
{
    // Projektion
    float3 proj = positionLightSpace.xyz / positionLightSpace.w;

    // NDC -> UV
    float2 uv;
    uv.x = proj.x * 0.5f + 0.5f;
    uv.y = -proj.y * 0.5f + 0.5f;

    // ausserhalb der Shadowmap -> kein Schatten
    if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f)
        return 1.0f;

    float currentDepth = proj.z;

    // Bias (slope-basiert, simpel aber wirksam)
    // normal: Surface-Normal, lightDir: Richtung des Lichts
    float ndotl = saturate(dot(normal, -lightDir));
    float bias = max(0.0008f * (1.0f - ndotl), 0.0005f);

    // Vergleichstiefe (kleiner => weniger self-shadow)
    float compareDepth = currentDepth - bias;

    // Texelgröße
    uint w, h;
    shadowMapTexture.GetDimensions(w, h);
    float2 texel = float2(1.0f / w, 1.0f / h);

    // 3x3 PCF
    float sum = 0.0f;
    [unroll]
    for (int y = -1; y <= 1; y++)
    {
        [unroll]
        for (int x = -1; x <= 1; x++)
        {
            float2 suv = uv + float2(x, y) * texel;
            sum += shadowMapTexture.SampleCmpLevelZero(shadowSampler, suv, compareDepth);
        }
    }

    return sum / 9.0f; // 1 = beleuchtet, 0 = im Schatten
}

// ==================== MAIN PIXEL SHADER ====================

float4 main(PS_INPUT input) : SV_Target
{
    float3 normal = normalize(input.normal);

    float3 ambient = float3(0.0, 0.0, 0.0);
    float3 diffuseAccum = float3(0.0, 0.0, 0.0);
    float3 specularAccum = float3(0.0, 0.0, 0.0);

    if (lightCount > 0)
        ambient = lights[0].lightAmbientColor.rgb;

    for (uint i = 0; i < lightCount; ++i)
    {
        float3 lightDir;
        float lightIntensity = 1.0f;
        float shadow = 1.0f;

        if (lights[i].lightPosition.w > 0.5f)
        {
            // POINT LIGHT
            float3 lightToPixel = input.worldPosition - lights[i].lightPosition.xyz;
            float distance = length(lightToPixel);
            lightDir = normalize(lightToPixel);

            float radius = (lights[i].lightDiffuseColor.a > 0.1f) ? lights[i].lightDiffuseColor.a : 100.0f;
            lightIntensity = CalculateLightFalloff(distance, radius);

            // Für Point-Lights hier KEIN Shadowmap-Test (bräuchte Cube-Shadowmaps)
            shadow = 1.0f;
        }
        else
        {
            // DIRECTIONAL LIGHT
            lightDir = normalize(lights[i].lightDirection.xyz);

            // Shadow nur für Directional Light
            shadow = ShadowPCF(input.positionLightSpace, normal, lightDir);
        }

        // Diffuse (Lambert)
        float diffuse_factor = max(dot(normal, -lightDir), 0.0f);
        diffuseAccum += lights[i].lightDiffuseColor.rgb * diffuse_factor * lightIntensity * shadow;

        // Specular (dein Placeholder)
        float3 halfVec = normalize(-lightDir + float3(0, 1, 0));
        float specular_factor = pow(max(dot(normal, halfVec), 0.0f), shininess);
        specularAccum += specularColor.rgb * specular_factor * lightIntensity * shadow;
    }

    float3 lighting = ambient + diffuseAccum + specularAccum;

    float4 texColor = textureMap.Sample(samplerState, input.texCoord);

    bool hasMaterialColor = (diffuseColor.r > 0.01 || diffuseColor.g > 0.01 || diffuseColor.b > 0.01);
    float3 matColor = hasMaterialColor ? diffuseColor.rgb : float3(1.0, 1.0, 1.0);

    float3 final_color;
    if (texColor.a > 0.0)
        final_color = texColor.rgb * matColor * lighting * input.color.rgb;
    else
        final_color = matColor * lighting * input.color.rgb;

    float finalAlpha = hasMaterialColor ? (diffuseColor.a * transparency) : 1.0;

    return float4(final_color, finalAlpha);
}
