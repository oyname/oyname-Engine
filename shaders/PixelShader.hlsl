// PixelShader.hlsl - Lighting + Shadow Mapping (PCF)
// Registers:
//  - t0/s0 : diffuse texture
//  - t7/s7 : shadow map (comparison sampler)   <<< FIXED (was t1/s1)
//  - b1    : LightBuffer
//  - b2    : MaterialBuffer

struct LightData
{
    float4 lightPosition; // XYZ: Position, W: 0 directional, 1 point
    float4 lightDirection; // XYZ: Direction (for directional lights)
    float4 lightDiffuseColor; // RGB: Diffuse, A: radius (for point lights)
    float4 lightAmbientColor; // RGB: Ambient (only first light used)
};

cbuffer LightBuffer : register(b1)
{
    LightData lights[32];
    uint lightCount;
    float3 lightPadding; // 16-byte alignment
};

cbuffer MaterialBuffer : register(b2)
{
    float4 diffuseColor;
    float4 specularColor;
    float shininess;
    float transparency;
    float receiveShadows; // 1 = use shadow map, 0 = ignore shadows
    float padding; // 16-byte alignment
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD0;
    float4 positionLightSpace : TEXCOORD2; // from VS (light clip space)
};

Texture2D textureMap : register(t0);
SamplerState samplerState : register(s0);

// Shadow map moved to t7/s7 to avoid collision with material multi-textures
Texture2D shadowMapTexture : register(t7);
SamplerComparisonState shadowSampler : register(s7);

// Point light falloff (simple quadratic)
float CalculateLightFalloff(float distance, float radius)
{
    float attenuation = max(0.0f, 1.0f - (distance / radius));
    return attenuation * attenuation;
}

// Shadow factor using PCF + comparison sampling.
// Returns 1.0 = lit, 0.0 = fully shadowed.
float CalculateShadowFactor(float4 positionLightSpace, float3 normal, float3 lightDir)
{
    // Avoid div-by-zero / invalid clip
    if (positionLightSpace.w <= 0.00001f)
        return 1.0f;

    // Perspective divide -> NDC
    float3 projCoords = positionLightSpace.xyz / positionLightSpace.w;

    // NDC [-1..1] -> texture [0..1]
    projCoords.x = projCoords.x * 0.5f + 0.5f;
    projCoords.y = -projCoords.y * 0.5f + 0.5f;

    // Outside shadow map => lit
    if (projCoords.x < 0.0f || projCoords.x > 1.0f ||
        projCoords.y < 0.0f || projCoords.y > 1.0f)
        return 1.0f;

    // Behind near/far => lit
    if (projCoords.z < 0.0f || projCoords.z > 1.0f)
        return 1.0f;

    // Slope-scaled-ish bias (cheap)
    // lightDir is direction from light->surface for directional (see C++), we use surface->light as -lightDir
    float ndotl = saturate(dot(normalize(normal), normalize(-lightDir)));
    float bias = max(0.0005f, 0.0030f * (1.0f - ndotl));

    float compareDepth = projCoords.z - bias;

    // PCF 3x3
    uint w, h;
    shadowMapTexture.GetDimensions(w, h);
    float2 texelSize = 1.0f / float2((float) w, (float) h);

    float shadowSum = 0.0f;
    [unroll]
    for (int y = -1; y <= 1; ++y)
    {
        [unroll]
        for (int x = -1; x <= 1; ++x)
        {
            float2 uv = projCoords.xy + float2((float) x, (float) y) * texelSize;
            // SampleCmp returns 1 if compareDepth <= sampledDepth (lit), else 0 (shadow)
            shadowSum += shadowMapTexture.SampleCmpLevelZero(shadowSampler, uv, compareDepth);
        }
    }

    return shadowSum / 9.0f;
}

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

        // W: 0 directional, 1 point
        if (lights[i].lightPosition.w > 0.5f)
        {
            float3 lightToPixel = input.worldPosition - lights[i].lightPosition.xyz;
            float distance = length(lightToPixel);
            lightDir = normalize(lightToPixel);

            float radius = (lights[i].lightDiffuseColor.a > 0.1f) ? lights[i].lightDiffuseColor.a : 100.0f;
            lightIntensity = CalculateLightFalloff(distance, radius);
        }
        else
        {
            lightDir = normalize(lights[i].lightDirection.xyz);
        }

        float shadowFactor = 1.0f;
        // Only first directional light casts shadow
        if (receiveShadows > 0.5f && i == 0 && lights[0].lightPosition.w < 0.5f)
            shadowFactor = CalculateShadowFactor(input.positionLightSpace, normal, lightDir);

        // Diffuse (Lambert) - your convention uses -lightDir for surface->light
        float diffuse_factor = max(dot(normal, -lightDir), 0.0f);
        diffuseAccum += lights[i].lightDiffuseColor.rgb * diffuse_factor * lightIntensity * shadowFactor;

        // Specular (placeholder half vector - keep your old behavior, but shadowed)
        float3 halfVec = normalize(-lightDir + float3(0, 1, 0));
        float specular_factor = pow(max(dot(normal, halfVec), 0.0f), shininess);
        specularAccum += specularColor.rgb * specular_factor * lightIntensity * shadowFactor;
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

