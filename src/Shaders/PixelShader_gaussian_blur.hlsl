struct PSInput
{
    float4 position : SV_POSITION;
};

struct FilterParams
{
    float4 blurOffsetKernelVariance;
};
ConstantBuffer<FilterParams> filterConstants : register(b0);

struct RootConstant
{
    uint filterDirection; // 0 = on X, 1 = on Y
};
ConstantBuffer<RootConstant> filterDir : register(b1);

Texture2D sourceTex : register(t0);
SamplerState baseSampler : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    int width, height, numMips;
    sourceTex.GetDimensions(0, width, height, numMips);

    float2 uvCoords = input.position.xy / width;

    float2 offset = filterConstants.blurOffsetKernelVariance.xx;
    if(filterDir.filterDirection == 0) {
        offset.y = 0;
    }
    else {
        offset.x = 0;
    }

    float4 color = float4(0.f, 0.f, 0.f, 0.f);
    // color += sourceTex.Sample(baseSampler, uvCoords + (-3.0.xx / (float)width * offset)) * (1.f / 64.f);
    // color += sourceTex.Sample(baseSampler, uvCoords + (-2.0.xx / (float)width * offset)) * (6.f / 64.f);
    // color += sourceTex.Sample(baseSampler, uvCoords + (-1.0.xx / (float)width * offset)) * (15.f / 64.f);
    // color += sourceTex.Sample(baseSampler, uvCoords + (0.0.xx / (float)width * offset))  * (20.f / 64.f);
    // color += sourceTex.Sample(baseSampler, uvCoords + (1.0.xx / (float)width * offset))  * (15.f / 64.f);
    // color += sourceTex.Sample(baseSampler, uvCoords + (2.0.xx / (float)width * offset))  * (6.f / 64.f);
    // color += sourceTex.Sample(baseSampler, uvCoords + (3.0.xx / (float)width * offset))  * (1.f / 64.f);
    float baseOffset = (filterConstants.blurOffsetKernelVariance.y - 1) / -2.f;
    float coord = baseOffset;
    float variance2 = filterConstants.blurOffsetKernelVariance.z * filterConstants.blurOffsetKernelVariance.z;
    [loop]
    for(int i = coord; i <= -coord; i++) {
        float2 sampleOffset = baseOffset.xx / (float)width * offset;
        float weight = 1.f / (sqrt(2 * 3.14 * variance2)) * exp(i * i / (-2.f * variance2));
        color += sourceTex.Sample(baseSampler, uvCoords + sampleOffset) * weight;
        baseOffset ++;
    }

    return color;
}