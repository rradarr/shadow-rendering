struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float4 lightPosition_viewSpace : LIGHT;
    float4 vertexPosition_viewSpace : VERTVIEW;
    float3 normal_viewSpace : NORMVIEW;
    float4 positionInLightProjection : LIGHTPROJPOS;
};

struct lightParams
{
    float3 lightPosition;
    float4 lightNearFarFrustumSize;
    float4 lightFilterKernelScaleBias;
    float4 mapAmbientSamplerBleed;
};
ConstantBuffer<lightParams> lightConstants : register(b1);

Texture2D albedoTex : register(t0);
SamplerState albedoSampler : register(s0);
Texture2D shadowMapMomentsTex : register(t1);
SamplerState shadowMapSamplerPoint : register(s1);
SamplerState shadowMapSamplerFiltered : register(s2);

float linstep(float min, float max, float v)
{
  return clamp((v - min) / (max - min), 0.f, 1.f);
}

float GetVarianceLitFactor(float3 samplePosInLightProjection) {
    const int SAMPLER_POINT = 0;
    const int SAMPLER_FILTERING = 1;
    const float MIN_VARIANCE = 0.00002f;
    float lightBleedingReduction = lightConstants.mapAmbientSamplerBleed.z;

    const float smapleZInLightProjection = samplePosInLightProjection.z;
    float2 moments = float2(0.f, 0.f);
    if(lightConstants.mapAmbientSamplerBleed.y == SAMPLER_POINT) {
        moments = shadowMapMomentsTex.Sample(shadowMapSamplerPoint, samplePosInLightProjection.xy).xy;
    }
    else {
        moments = shadowMapMomentsTex.Sample(shadowMapSamplerFiltered, samplePosInLightProjection.xy).xy;
    }
    // One-tailed inequality valid only if z > moments.x
    float p = (smapleZInLightProjection <= moments.x);
    // Compute variance.
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, MIN_VARIANCE);
    // Compute probabilistic upper bound.
    float d = smapleZInLightProjection - moments.x;
    float p_max = variance / (variance + d * d);
    p_max = linstep(lightBleedingReduction, 1.f, p_max);

    // Return the Chebyshev upper bound.
    return saturate(max(p, p_max));
}
 
float4 main(PSInput input) : SV_TARGET
{
    const float ambientBrightness = lightConstants.mapAmbientSamplerBleed.x; // The shadows will not get darker than this.

    float3 lightDirection_viewSpace = normalize((input.lightPosition_viewSpace - input.vertexPosition_viewSpace).xyz);
    float diffuseStrength = saturate(dot(input.normal_viewSpace.xyz, lightDirection_viewSpace));

    // Move the light space sample position into NDC by performing perspective divide.
    input.positionInLightProjection.xyz /= input.positionInLightProjection.w;
    // input.positionInLightProjection.z += lightConstants.lightFilterKernelScaleBias.w; // Add manual depth bias.
    
    float litFactor = 0.f;
    litFactor = GetVarianceLitFactor(input.positionInLightProjection.xyz);
    litFactor = saturate(litFactor + ambientBrightness);

    float4 surfaceColor = albedoTex.Sample(albedoSampler, input.texCoord) * input.color;
    float4 litColor = surfaceColor * float4(max(diffuseStrength, ambientBrightness).xxx, 1);
    float4 shadowColor = surfaceColor * ambientBrightness;

    return lerp(shadowColor, litColor, litFactor);
}