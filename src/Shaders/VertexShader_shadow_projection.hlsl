struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

struct WvpMatrices
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;
};
ConstantBuffer<WvpMatrices> matrices : register(b0);

struct LightParams
{
    float3 lightPosition;
};
ConstantBuffer<LightParams> lightConstants : register(b1);

const float4x4 GetPlaneProjectionMatrix(float3 lightPosition)
{
    float lx = lightPosition.x;
    float ly = lightPosition.y;
    float lz = lightPosition.z;
    const float4x4 mat = {
        ly,    -lx,    0,      0,
        0,     0,      0,      0,
        0,     -lz,    ly,     0,
        0,     -1,     0,      ly
    };
    
    return mat;
}

PSInput main(float4 position : POSITION, float4 color : COLOR, float2 uv : UV, float3 normal : NORMAL)
{
    PSInput result;

    // Transform vertices into world space.
    const float4 vertexPosition_worldSpace = mul(position, matrices.worldMatrix);
    
    // Project vertices onto y=0.
    // Prepare the projection matrix.
    float4x4 mat = GetPlaneProjectionMatrix(lightConstants.lightPosition);
    float4 projectedVertexPosition = mul(mat, vertexPosition_worldSpace);
    
    // Transform with view and projection.
    result.position = mul(projectedVertexPosition, matrices.viewMatrix);
    result.position = mul(result.position, matrices.projMatrix);

    result.color = color;
    result.texCoord = uv;
    result.normal = normal;

    return result;
}