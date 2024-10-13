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

struct wvpMatrixValue
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;
};
ConstantBuffer<wvpMatrixValue> objectWVP : register(b0);
ConstantBuffer<wvpMatrixValue> lightWVP : register(b2);

struct lightParams
{
    float3 lightPosition;
};
ConstantBuffer<lightParams> lightConstants : register(b1);


PSInput main(float4 position : POSITION, float4 color : COLOR, float2 uv : UV, float3 normal : NORMAL)
{
    PSInput result;

    // Calculate components for light calculations.
    float4 vertexPosition_worldSpace = mul(position, objectWVP.worldMatrix);
    float4 vertexPosition_viewSpace = mul(vertexPosition_worldSpace, objectWVP.viewMatrix);
    result.vertexPosition_viewSpace = vertexPosition_viewSpace;
    result.position = mul(vertexPosition_viewSpace, objectWVP.projMatrix);

    float4 normal_worldSpace = normalize(mul(float4(normal, 0), objectWVP.worldMatrix));
    float3 normal_viewSpace = normalize(mul(normal_worldSpace, objectWVP.viewMatrix));
    result.normal_viewSpace = normal_viewSpace;

    float4 lightPosition_viewSpace = mul(float4(lightConstants.lightPosition, 1), objectWVP.viewMatrix);
    result.lightPosition_viewSpace = lightPosition_viewSpace;

    // Calculate position in light space.
    result.positionInLightSpace = mul(vertexPosition_worldSpace, lightWVP.viewMatrix);
    result.positionInLightSpace = mul(result.positionInLightSpace, lightWVP.projMatrix);

    result.color = color;
    result.texCoord = uv;
    result.normal = normal;

    return result;
}