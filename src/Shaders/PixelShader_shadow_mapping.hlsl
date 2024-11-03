struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float4 lightPosition_viewSpace : LIGHT;
    float4 vertexPosition_viewSpace : VERTVIEW;
    float3 normal_viewSpace : NORMVIEW;
    float4 positionInLightView : LIGHTVIEWPOS;
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
Texture3D pcfOffsetsTex : register(t2);
SamplerState albedoSampler : register(s0);
Texture2D shadowMapTex : register(t1);
SamplerComparisonState shadowMapSamplerPoint : register(s1);
SamplerComparisonState shadowMapSamplerBilinear : register(s2);


// Return [0:1] value telling how much teh sample is lit (0 = in shadow, 1 = fully lit).
float GetPCFManualFilterLitPercentage(float3 lightSpaceSamplePos, int kernelSize, float offsetScale, float samplerToUse) {
    // PCF shadow map - manual filtering.
    // Find texel size.
    uint width, height, numMips;
    shadowMapTex.GetDimensions(0, width, height, numMips);
    const float dx = 1.0f / (float)width * offsetScale;
    const float dy = 1.0f / (float)height * offsetScale;

    float lit = 0.0f;
    const int maxKernelSize = 11;
    float2 offsets[maxKernelSize*maxKernelSize];
    // const float2 offsets[kernelSize*kernelSize] =
    // {
    //     float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
    //     float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
    //     float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
    // };
    // const int kernelSize = 5;
    // const float2 offsets[kernelSize*kernelSize] =
    // {
    //     float2(-2.f*dx,  -2.f*dx), float2(-dx,  -2.f*dx), float2(0.0f, -2.f*dx), float2(dx, -2.f*dx), float2(2.f*dx, -2.f*dx),
    //     float2(-2.f*dx,  -dx), float2(-dx,  -dx), float2(0.0f, -dx), float2(dx, -dx), float2(2.f*dx, -dx),
    //     float2(-2.f*dx,  0.f), float2(-dx,  0.f), float2(0.0f, 0.0f), float2(dx, 0.0f), float2(2.f*dx, 0.0f),
    //     float2(-2.f*dx,  dx), float2(-dx,  dx), float2(0.0f, dx), float2(dx, dx), float2(2.f*dx, dx),
    //     float2(-2.f*dx,  2.f*dx), float2(-dx,  2.f*dx), float2(0.0f, 2.f*dx), float2(dx, 2.f*dx), float2(2.f*dx, 2.f*dx)
    // };

    // Calculate the offsets.
    float yOffset = dy * (kernelSize - 1) / -2.f;
    [loop]
    for(int r = 0; r < kernelSize; r++) {
        float xOffset = dx * (kernelSize - 1) / -2.f;
        [loop]
        for(int c = 0; c < kernelSize; c++) {
            
            offsets[c + r*kernelSize] = float2(xOffset,  yOffset);
            xOffset += dx;
        }
        yOffset += dy;
    }

    // Silly way to dynamically choose the sampler to use...
    if(samplerToUse == 0.f) {
        [loop]
        for(int i = 0; i < kernelSize*kernelSize; ++i)
        {
            lit += saturate(
                shadowMapTex.SampleCmpLevelZero(
                    shadowMapSamplerPoint,
                    lightSpaceSamplePos.xy + offsets[i],
                    lightSpaceSamplePos.z).x
            );
        }
    }
    else {
        [loop]
        for(int i = 0; i < kernelSize*kernelSize; ++i)
        {
            lit += saturate(
                shadowMapTex.SampleCmpLevelZero(
                    shadowMapSamplerBilinear,
                    lightSpaceSamplePos.xy + offsets[i],
                    lightSpaceSamplePos.z).x
            );
        }
    }
    
    lit /= kernelSize*kernelSize;

    return lit;
}

float GetPCFRandomFilterLitPercentage(float3 lightSpaceSamplePos, float2 screenSpacePos, float offsetScale, float samplerToUse) {
    int widthOffsets, heightOffsets, depthOffsets, numMipsOffsets;
    pcfOffsetsTex.GetDimensions(0, depthOffsets, widthOffsets, heightOffsets, numMipsOffsets);

    // Divide by texture size to get one texel per pixel.
    float2 offsetWindowIndex = float2(screenSpacePos / (float)widthOffsets);
    float offsetTexelSize = 1.f / (float)depthOffsets;

    // Prepare for PCF shadow map lookups.
    float2 shadowMapUV = lightSpaceSamplePos.xy;
    const float2 mapTexelSize = float2(1.f, 1.f);

    // Test the outer samples.
    // Here the example implementation takes a hardcoded amount of loops.
    // We can programmatically set the loop to use all outer samples (edge of filter size, so sqrt(depth)).
    float lit = 0.f;
    int outerOffsetCount = int(sqrt(2.f * (float)depthOffsets) / 2);
    [loop]
    for(int i = 0; i < outerOffsetCount; i++) {
        // Get offsets (two packed into xy and zw).
        float4 offsets = pcfOffsetsTex.Sample(
            albedoSampler,
            float3(i * offsetTexelSize, offsetWindowIndex));

        offsets = offsets * mapTexelSize.xyxy * offsetScale;

        // Test samples.
        if(samplerToUse == 0.f) {
            lit += saturate(
                shadowMapTex.SampleCmpLevelZero(
                    shadowMapSamplerPoint,
                    shadowMapUV + offsets.xy,
                    lightSpaceSamplePos.z).x
            );
            lit += saturate(
                shadowMapTex.SampleCmpLevelZero(
                    shadowMapSamplerPoint,
                    shadowMapUV + offsets.zw,
                    lightSpaceSamplePos.z).x
            );
        }
        else {
            lit += saturate(
                shadowMapTex.SampleCmpLevelZero(
                    shadowMapSamplerBilinear,
                    shadowMapUV + offsets.xy,
                    lightSpaceSamplePos.z).x
            );
            lit += saturate(
                shadowMapTex.SampleCmpLevelZero(
                    shadowMapSamplerBilinear,
                    shadowMapUV + offsets.zw,
                    lightSpaceSamplePos.z).x
            );
        }
    }
    // If all the same, return lit or shadowed.
    // Rmember that in each outerOffset we have two offsets, in xy and zw!
    float litPercentage = lit / (outerOffsetCount * 2.f);
    if(litPercentage == 0.f || litPercentage == 1.f) {
        return litPercentage;
    }

    // Else continue testing rest of samples.
    [loop]
    for(int i = outerOffsetCount; i < depthOffsets; i++) {
        // Get offsets (two packed into xy and zw).
        float4 offsets = pcfOffsetsTex.Sample(
            albedoSampler,
            float3(i * offsetTexelSize, offsetWindowIndex));

        offsets = offsets * mapTexelSize.xyxy * offsetScale;

        // Test samples.
        if(samplerToUse == 0.f) {
            lit += saturate(
                shadowMapTex.SampleCmpLevelZero(
                    shadowMapSamplerPoint,
                    shadowMapUV + offsets.xy,
                    lightSpaceSamplePos.z).x
            );
            lit += saturate(
                shadowMapTex.SampleCmpLevelZero(
                    shadowMapSamplerPoint,
                    shadowMapUV + offsets.zw,
                    lightSpaceSamplePos.z).x
            );
        }
        else {
            lit += saturate(
                shadowMapTex.SampleCmpLevelZero(
                    shadowMapSamplerBilinear,
                    shadowMapUV + offsets.xy,
                    lightSpaceSamplePos.z).x
            );
            lit += saturate(
                shadowMapTex.SampleCmpLevelZero(
                    shadowMapSamplerBilinear,
                    shadowMapUV + offsets.zw,
                    lightSpaceSamplePos.z).x
            );
        }
    }

    return lit / (depthOffsets * 2.f);
}

float FindAverageBlockerDistance(float3 lightSpaceSamplePos, float searchSize, float2 screenSpacePos) {  
    // Search for blockers.
    float BLOCKER_BIAS = 0.f; // TODO: supply this from app.
    int blockersCount = 0;
    float blockerDistance = 0.f;

    int widthOffsets, heightOffsets, depthOffsets, numMipsOffsets;
    pcfOffsetsTex.GetDimensions(0, depthOffsets, widthOffsets, heightOffsets, numMipsOffsets);
    // Divide by texture size to get one texel per pixel.
    float2 offsetWindowIndex = float2(screenSpacePos / (float)widthOffsets) + 1.1532f;
    float offsetTexelSize = 1.f / (float)depthOffsets;
    int outerOffsetCount = int(sqrt(2.f * (float)depthOffsets) / 2);
    
    [loop]
    for(int i = 0; i < outerOffsetCount; i++) {
        // Get outer offets.
        float4 offsets = pcfOffsetsTex.Sample(
            albedoSampler,
            float3(i * offsetTexelSize, offsetWindowIndex));

        offsets *= searchSize;

        float depth = shadowMapTex.Sample(albedoSampler, lightSpaceSamplePos.xy + offsets.xy).x;
        if(depth < lightSpaceSamplePos.z - BLOCKER_BIAS) {
            blockersCount ++;
            blockerDistance += depth;
        }

        depth = shadowMapTex.Sample(albedoSampler, lightSpaceSamplePos.xy + offsets.zw).x;
        if(depth < lightSpaceSamplePos.z - BLOCKER_BIAS) {
            blockersCount ++;
            blockerDistance += depth;
        }
    }

    if (blockersCount == 0) {
        return -1;
    }

    [loop]
    for(int i = outerOffsetCount; i < depthOffsets; i++) {
    // for(int i = 0; i < depthOffsets; i++) {
        // Get outer offets.
        float4 offsets = pcfOffsetsTex.Sample(
            albedoSampler,
            float3(i * offsetTexelSize, offsetWindowIndex));

        offsets *= searchSize;

        float depth = shadowMapTex.Sample(albedoSampler, lightSpaceSamplePos.xy + offsets.xy).x;
        if(depth < lightSpaceSamplePos.z - BLOCKER_BIAS) {
            blockersCount ++;
            blockerDistance += depth;
        }

        depth = shadowMapTex.Sample(albedoSampler, lightSpaceSamplePos.xy + offsets.zw).x;
        if(depth < lightSpaceSamplePos.z - BLOCKER_BIAS) {
            blockersCount ++;
            blockerDistance += depth;
        }
    }

    // Calculate average depth.
    if(blockersCount > 0) {
        return blockerDistance / (float)blockersCount;
        // TODO: for debugging
        // return (float)blockersCount / 64.f;
    }
    else {
        return -2;
    }
}
 
float4 main(PSInput input) : SV_TARGET
{
    const float2 screenSize = float2(1500.f, 1000.f);
    const float ambientBrightness = lightConstants.mapAmbientSamplerBleed.x; // The shadows will not get darker than this.

    float3 lightDirection_viewSpace = normalize((input.lightPosition_viewSpace - input.vertexPosition_viewSpace).xyz);
    float diffuseStrength = saturate(dot(input.normal_viewSpace.xyz, lightDirection_viewSpace));

    // Move the light space sample position into NDC by performing perspective divide.
    input.positionInLightProjection.xyz /= input.positionInLightProjection.w;
    input.positionInLightProjection.z += lightConstants.lightFilterKernelScaleBias.w; // Add manual depth bias.
    
    float2 shadowMapUV = input.positionInLightProjection.xy;
    float litFactor = 0.f;

    // TODO: for debug.
    float avgBlockerDistance = 0.f;
    float penumbraWidth = 0.f;

    if(lightConstants.lightFilterKernelScaleBias.x == 0) {
        // No filtering.
        if(lightConstants.mapAmbientSamplerBleed.y == 0.f) {
            litFactor = saturate(shadowMapTex.SampleCmpLevelZero(shadowMapSamplerPoint, shadowMapUV, input.positionInLightProjection.z).x);
        }
        else {
            litFactor = saturate(shadowMapTex.SampleCmpLevelZero(shadowMapSamplerBilinear, shadowMapUV, input.positionInLightProjection.z).x);
        }
    }
    else if(lightConstants.lightFilterKernelScaleBias.x == 1) {
        // Manually built filter.
        litFactor = GetPCFManualFilterLitPercentage(
            input.positionInLightProjection.xyz,
            lightConstants.lightFilterKernelScaleBias.y,
            lightConstants.lightFilterKernelScaleBias.z,
            lightConstants.mapAmbientSamplerBleed.y);
    }
    else if(lightConstants.lightFilterKernelScaleBias.x == 2) {
        // Random offset filter.
        if(diffuseStrength != 0.f) {
            uint widthMap, heightMap, numMipsMap;
            shadowMapTex.GetDimensions(0, widthMap, heightMap, numMipsMap);
            const float2 mapTexelSize = float2(1.0f / (float)widthMap, 1.0f / (float)heightMap);

            litFactor = GetPCFRandomFilterLitPercentage(
                input.positionInLightProjection.xyz,
                input.position.xy,
                lightConstants.lightFilterKernelScaleBias.z * mapTexelSize.x,
                lightConstants.mapAmbientSamplerBleed.y);
        }
    }
    else if(lightConstants.lightFilterKernelScaleBias.x == 3) {
        if(diffuseStrength != 0.f) {
            // PCSS.

            // Find search size.
            const float NEAR_LIGHT_PLANE = lightConstants.lightNearFarFrustumSize.x;
            const float LIGHT_FRUSTUM_WIDTH = lightConstants.lightNearFarFrustumSize.z;
            const float worldLightSize = lightConstants.lightNearFarFrustumSize.w;
            float lightSizeInLightSpace = worldLightSize / LIGHT_FRUSTUM_WIDTH;
            float searchSize = lightSizeInLightSpace * (input.positionInLightView.z - NEAR_LIGHT_PLANE) / input.positionInLightView.z;
            // float searchSize = lightSizeInLightSpace * input.positionInLightProjection.z / (input.positionInLightProjection.z + NEAR_LIGHT_PLANE);

            // Find occluders and avg occluder depth.
            avgBlockerDistance = FindAverageBlockerDistance(
                input.positionInLightProjection.xyz,
                searchSize,
                input.position.xy);
            if (avgBlockerDistance < 0.f) { // If no blockers, avgBlockerDistance will be -1.
                // Fully lit if not blocked.
                litFactor = 1.f;
                // litFactor = avgBlockerDistance;
            }
            else {
                // Calculate filter width.
                penumbraWidth = (input.positionInLightProjection.z - avgBlockerDistance) / avgBlockerDistance;
                float filterSize = penumbraWidth * lightSizeInLightSpace * NEAR_LIGHT_PLANE / input.positionInLightProjection.z;
                // Use filter to get litFactor.
                litFactor = GetPCFRandomFilterLitPercentage(
                    input.positionInLightProjection.xyz,
                    input.position.xy,
                    filterSize * lightConstants.lightFilterKernelScaleBias.z,
                    lightConstants.mapAmbientSamplerBleed.y);
            }
        }
    }
    // TODO: for debug
    // return float4(penumbraWidth.xxx, 1.f);
    // if(litFactor == -1.f) {
    //     // Early out at occulder search.
    //     return float4(0.f, 0.5f, 0.f, 1.f);
    // }
    // else if(litFactor == -2.f) {
    //     // Late out at occulder search.
    //     return float4(0.5f, 0.5f, 0.f, 1.f);
    // }

    litFactor = saturate(litFactor + ambientBrightness);

    float4 surfaceColor = albedoTex.Sample(albedoSampler, input.texCoord) * input.color;
    float4 litColor = surfaceColor * float4(max(diffuseStrength, ambientBrightness).xxx, 1);
    float4 shadowColor = surfaceColor * ambientBrightness;

    return lerp(shadowColor, litColor, litFactor);
}