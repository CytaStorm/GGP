#include "ShaderIncludes.hlsli"
Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);

cbuffer PostProcessChromaticAbberationConstantBuffer : register(b0)
{
    float3 offset;
    float buffer;
    float2 mouseFocusPoint;
}

float4 main(VertexToPixel input) : SV_TARGET
{
    int width;
    int height;
    Pixels.GetDimensions(width, height);
    float2 textureSize = { width, height };
    float2 texCoord = input.screenPosition.xy / textureSize;
    float2 direction = texCoord - mouseFocusPoint;

    float4 finalColor = Pixels.Sample(ClampSampler, input.uv);
    finalColor.r = Pixels.Sample(ClampSampler, input.uv + (direction * offset.r)).r;
    finalColor.g = Pixels.Sample(ClampSampler, input.uv + (direction * offset.g)).g;
    finalColor.b = Pixels.Sample(ClampSampler, input.uv + (direction * offset.b)).b;

    return finalColor;
}