#include "ShaderIncludes.hlsli"

Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);

cbuffer PostProcessGaussVerticalConstantBuffer : register(b0)
{
    int blurRadius;
    float pixelHeight;
};

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 pixelColor = 0;
    int sampleCount = 0;

     
    for (int i = -blurRadius; i <= blurRadius; i++)
    {
        float2 uv = input.uv;
        uv.y += i * pixelHeight;

        pixelColor += Pixels.Sample(ClampSampler, uv);
        sampleCount++;
    }
    
    //= Pixels.Sample(ClampSampler, input.uv);
    // NOTE: Here is where you should actually "process" the image

    return pixelColor / sampleCount;
}