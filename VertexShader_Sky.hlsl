#include "ShaderIncludes.hlsli"

cbuffer BufferStruct : register(b0)
{
    matrix projection;
    matrix view;
}

VertexToPixel_Sky main( VertexShaderInput input )
{
    VertexToPixel_Sky output;
    matrix viewNoTranslate = view;
    viewNoTranslate._14 = 0;
    viewNoTranslate._24 = 0;
    viewNoTranslate._34 = 0;

    output.position = mul(mul(projection, viewNoTranslate), float4(input.localPosition, 1));
    output.position.z = output.position.w;
    output.sampleDir = input.localPosition;

    return output;
}