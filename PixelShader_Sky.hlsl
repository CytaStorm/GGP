#include "ShaderIncludes.hlsli"

Texture2D SkyTexture : register(t0);
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel_Sky input)
{
    return SkyTexture.Sample(BasicSampler, input.sampleDir.xy);
}