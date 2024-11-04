struct PSInput
{
    float4 position : SV_POSITION;
};

PSInput main(float4 position : POSITION, float4 color : COLOR, float2 uv : UV, float3 normal : NORMAL)
{
    PSInput result;

    // Just output the fullscreen quad (has to have -1:1 in x and y coords).
    result.position = float4(position.xy, 0, 1);
    return result;
}