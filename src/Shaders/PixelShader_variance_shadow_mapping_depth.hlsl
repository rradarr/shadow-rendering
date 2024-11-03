struct PSInput
{
    float4 position : SV_POSITION;
};

float2 main(PSInput input) : SV_TARGET
{
    float2 moments;
    const float depth = input.position.z;
    // First moment is the depth itself.
    moments.x = depth;
    // Compute partial derivatives of depth.
    float dx = ddx(depth);
    float dy = ddy(depth);
    // Compute second moment over the pixel extents.
    moments.y = depth * depth + 0.25 * (dx * dx + dy * dy);
    return moments;
}