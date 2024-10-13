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
SamplerState shadowMapSampler : register(s1);

float4 main(PSInput input) : SV_TARGET
{
    const float ambientBrightness = 0.02; // The shadows will not get darker than this.

    float3 lightDirection_viewSpace = normalize((input.lightPosition_viewSpace - input.vertexPosition_viewSpace).xyz);
    float3 diffuseStrength = saturate(dot(input.normal_viewSpace.xyz, lightDirection_viewSpace));

    // Calculate the shadow.
    input.positionInLightSpace.xyz /= input.positionInLightSpace.w;
    // Move the xy coordiantes from [-1;1] to UV [0;1] and flip the Y axis.
    float2 shadowMapUV = (input.positionInLightSpace.xy / float2(2.0, -2.0)) + 0.5;
    float shadowMapValue = shadowMapTex.Sample(shadowMapSampler, shadowMapUV).r;
    float lit = step(input.positionInLightSpace.z, shadowMapValue) * (1 - ambientBrightness) + ambientBrightness;

    float4 surfaceColor = albedoTex.Sample(albedoSampler, input.texCoord) * input.color;
    float4 litColor = surfaceColor * float4(max(diffuseStrength, ambientBrightness), 1);
    float4 shadowColor = surfaceColor * ambientBrightness;
    return lerp(shadowColor, litColor, lit);
}