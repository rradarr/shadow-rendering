struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float4 lightPosition_viewSpace : LIGHT;
    float4 vertexPosition_viewSpace : VERTVIEW;
    float3 normal_viewSpace : NORMVIEW;
};

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    const float ambientBrightness = 0.02; // The shadows will not get darker than this.

    float3 lightDirection_viewSpace = normalize((input.lightPosition_viewSpace - input.vertexPosition_viewSpace).xyz);
    float3 diffuseStrength = saturate(dot(input.normal_viewSpace.xyz, lightDirection_viewSpace));

    float4 surfaceColor = t0.Sample(s0, input.texCoord) * input.color;
    return surfaceColor * float4(max(diffuseStrength, ambientBrightness), 1);
}