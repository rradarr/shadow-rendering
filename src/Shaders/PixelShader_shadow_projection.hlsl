struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

float4 main(PSInput input) : SV_TARGET
{
    return float4(0.f, 0.f, 0.f, 1.f);
}