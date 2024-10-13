struct PSInput
{
    float4 position : SV_POSITION;
};

struct WvpMatrices
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;
};
ConstantBuffer<WvpMatrices> objectWVP : register(b0);
ConstantBuffer<WvpMatrices> lightWVP : register(b1);

PSInput main(float4 position : POSITION, float4 color : COLOR, float2 uv : UV, float3 normal : NORMAL)
{
    PSInput result;

    // Get object World, light View and light Projection.
    float4 outPosition = mul(position, objectWVP.worldMatrix);
    outPosition = mul(outPosition, lightWVP.viewMatrix);
    outPosition = mul(outPosition, lightWVP.projMatrix);

    result.position = outPosition;
    return result;
}