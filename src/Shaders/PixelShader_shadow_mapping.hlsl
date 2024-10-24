struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float4 lightPosition_viewSpace : LIGHT;
    float4 vertexPosition_viewSpace : VERTVIEW;
    float3 normal_viewSpace : NORMVIEW;
    float4 positionInLightSpace : LIGHTVIEWPOS;
};

Texture2D albedoTex : register(t0);
SamplerState albedoSampler : register(s0);
Texture2D shadowMapTex : register(t1);
SamplerComparisonState shadowMapSampler : register(s1);

// Return [0:1] value telling how much teh sample is lit (0 = in shadow, 1 = fully lit).
float GetLitPercentage(float4 lightSpaceSamplePos) {
    // Move the light space sample position into NDC by performing perspective divide.
    lightSpaceSamplePos.xyz /= lightSpaceSamplePos.w;

    // Move the xy coordiantes from [-1;1] to UV [0;1] and flip the Y axis (already done during shadow map generation).
    // float2 shadowMapUV = (input.positionInLightSpace.xy / float2(2.0, -2.0)) + 0.5;
    float2 shadowMapUV = lightSpaceSamplePos.xy;

    // Basic shadow map - no filtering (requires appropriate linear filter to be set in shadowMapSampler).
    // float shadowMapValue = shadowMapTex.Sample(shadowMapSampler, shadowMapUV).r;
    // float lit = step(input.positionInLightSpace.z, shadowMapValue)

    // PCF shadow map - hardware linear filering (requires a comparison function on the sampler and a comparison filter).
    // float lit = saturate(shadowMapTex.SampleCmpLevelZero(shadowMapSampler, shadowMapUV, lightSpaceSamplePos.z).x);

    // PCF shadow map - manual filtering.
    // Find texel size.
    uint width, height, numMips;
    shadowMapTex.GetDimensions(0, width, height, numMips);
    float dx = 1.0f / (float)width;

    float lit = 0.0f;
    // const int kernelSize = 3;
    // const float2 offsets[kernelSize*kernelSize] =
    // {
    //     float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
    //     float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
    //     float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
    // };
    const int kernelSize = 5;
    const float2 offsets[kernelSize*kernelSize] =
    {
        float2(-2.f*dx,  -2.f*dx), float2(-dx,  -2.f*dx), float2(0.0f, -2.f*dx), float2(dx, -2.f*dx), float2(2.f*dx, -2.f*dx),
        float2(-2.f*dx,  -dx), float2(-dx,  -dx), float2(0.0f, -dx), float2(dx, -dx), float2(2.f*dx, -dx),
        float2(-2.f*dx,  0.f), float2(-dx,  0.f), float2(0.0f, 0.0f), float2(dx, 0.0f), float2(2.f*dx, 0.0f),
        float2(-2.f*dx,  dx), float2(-dx,  dx), float2(0.0f, dx), float2(dx, dx), float2(2.f*dx, dx),
        float2(-2.f*dx,  2.f*dx), float2(-dx,  2.f*dx), float2(0.0f, 2.f*dx), float2(dx, 2.f*dx), float2(2.f*dx, 2.f*dx)
    };

    [unroll]
    for(int i = 0; i < kernelSize*kernelSize; ++i)
    {
        lit += saturate(
            shadowMapTex.SampleCmpLevelZero(
                shadowMapSampler,
                lightSpaceSamplePos.xy + offsets[i],
                lightSpaceSamplePos.z).x
        );
    }
    lit = lit / (kernelSize*kernelSize);

    return lit;
}
 
float4 main(PSInput input) : SV_TARGET
{
    const float ambientBrightness = 0.02; // The shadows will not get darker than this.

    float3 lightDirection_viewSpace = normalize((input.lightPosition_viewSpace - input.vertexPosition_viewSpace).xyz);
    float3 diffuseStrength = saturate(dot(input.normal_viewSpace.xyz, lightDirection_viewSpace));

    // Raise the factor up a bit when in shadow, to give some shape-defining diffuse even in shadows.
    float litFactor = saturate(GetLitPercentage(input.positionInLightSpace) + ambientBrightness);

    float4 surfaceColor = albedoTex.Sample(albedoSampler, input.texCoord) * input.color;
    float4 litColor = surfaceColor * float4(max(diffuseStrength, ambientBrightness), 1);
    float4 shadowColor = surfaceColor * ambientBrightness;
    return lerp(shadowColor, litColor, litFactor);
}